#!/usr/bin/env python3
"""
USB_Tool 上位机测试工具
功能: 串口连接、模式切换、CAN ID 管理、CAN帧收发、自动模式识别、Bootloader升级
"""

import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext, filedialog
import serial
import serial.tools.list_ports
import threading
import time
import struct
import os
import datetime
import traceback
import queue

# ===================== 实时日志文件 =====================
LOG_FILE_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "usb_tool_live.log")

# ===================== CAN 桥接常量 =====================
MODE_LEGACY          = bytes([0xAA, 0xAA, 0xAA])
MODE_PROTOCOL_BRIDGE = bytes([0xBB, 0xBB, 0xBB])

FRAME_PROTOCOL_LEN   = 65
FRAME_CTRL_LEN       = 5
FRAME_CTRL_MAGIC     = bytes([0xA5, 0xA5])

CAN_RX_FRAME_LEN     = 12  # 固件回传CAN帧长度: 4B ID + 8B 数据

# ===================== J1939 协议常量 =====================
# 地址
BL_ADDR_PC           = 0xFF    # PC工具地址 (与正常上位机一致)
BL_ADDR_DEVICE       = 0x01    # 目标设备地址
BL_ADDR_BROADCAST    = 0xFF

# PF (PDU Format)
PF_BOOT_CMD          = 224     # 0xE0
PF_BOOT_JUMP_APP     = 225     # 0xE1  跳转APP/重启命令
PF_BOOT_JUMP_B       = 228     # 0xE4
PF_BOOT_UPDATA_B     = 229     # 0xE5
PF_HEARTBEAT         = 231     # 0xE7
PF_ACK               = 232     # 0xE8
PF_REQUEST           = 234     # 0xEA
PF_TP_DT             = 235     # 0xEB
PF_TP_CM             = 236     # 0xEC

# PGN
PGN_BOOT_CMD         = 0xE000
PGN_ACK              = 0xE800

# 优先级 (与正常上位机一致)
PRIORITY_BOOT_CMD    = 1
PRIORITY_TP          = 1

# BOOT 命令
BOOT_CMD_START       = 0x02
BOOT_CMD_VERIFY      = 0x03

# TP 控制字
TP_CM_RTS            = 0x10
TP_CM_CTS            = 0x11
TP_CM_END_ACK        = 0x13

# ACK 类型
ACK_POSITIVE         = 0x00
ACK_NEGATIVE         = 0x01

# Bootloader 状态
BL_STATE_IDLE        = 0
BL_STATE_QUERY_DONE  = 1
BL_STATE_READY       = 2
BL_STATE_PROGRAMMING = 3
BL_STATE_VERIFY      = 4
BL_STATE_COMPLETE    = 5
BL_STATE_ERROR       = 6

BL_STATE_NAMES = {
    BL_STATE_IDLE:        "空闲",
    BL_STATE_QUERY_DONE:  "查询完成",
    BL_STATE_READY:       "准备接收",
    BL_STATE_PROGRAMMING: "编程中",
    BL_STATE_VERIFY:      "校验中",
    BL_STATE_COMPLETE:    "升级完成",
    BL_STATE_ERROR:       "错误",
}


# ===================== J1939 工具函数 =====================

def j1939_build_id(priority: int, pf: int, ps: int, sa: int) -> int:
    """构建 J1939 29-bit CAN ID (DP=1, 与CAN_OTA协议一致)"""
    return ((priority & 0x07) << 26) | (1 << 24) | ((pf & 0xFF) << 16) | ((ps & 0xFF) << 8) | (sa & 0xFF)


def j1939_parse_id(can_id: int) -> dict:
    """解析 J1939 29-bit CAN ID"""
    return {
        "priority": (can_id >> 26) & 0x07,
        "pf":       (can_id >> 16) & 0xFF,
        "ps":       (can_id >> 8)  & 0xFF,
        "sa":       can_id & 0xFF,
    }


def checksum16(data: bytes) -> int:
    """计算16位累加和校验"""
    return sum(data) & 0xFFFF


def build_can_frame(can_id: int, data: bytes) -> bytes:
    """构建12字节CAN帧 (4B ID大端 + 8B 数据)"""
    header = bytes([(can_id >> 24) & 0xFF, (can_id >> 16) & 0xFF,
                    (can_id >> 8) & 0xFF, can_id & 0xFF])
    payload = data[:8].ljust(8, b'\x00')
    return header + payload


# ===================== 深色主题配色 (Catppuccin Mocha) =====================
BG          = "#1e1e2e"
BG_DARK     = "#181825"
BG_WIDGET   = "#313244"
BG_HOVER    = "#45475a"
FG          = "#cdd6f4"
FG_DIM      = "#a6adc8"
FG_Muted    = "#7f849c"
ACCENT      = "#89b4fa"
GREEN       = "#a6e3a1"
ORANGE      = "#fab387"
RED         = "#f38ba8"
YELLOW      = "#f9e2af"
BORDER      = "#45475a"


# ===================== 主窗口 =====================
class USB_Tool_Tester:
    def __init__(self, root):
        self.root = root
        self.root.title("USB_Tool 上位机测试工具")
        self.root.geometry("960x780")
        self.root.resizable(True, True)

        self.ser: serial.Serial | None = None
        self.read_thread: threading.Thread | None = None
        self.running = False

        # Bootloader 状态
        self._bl_state = BL_STATE_IDLE
        self._bl_fw_data = b""
        self._bl_running = False
        self._bl_stop = False           # 停止升级标志
        self._bl_rx_queue = queue.Queue()  # 接收队列, 替代Event等待
        self._bl_ack_event = threading.Event()
        self._bl_ack_result = None      # (ack_type, pgn)
        self._bl_ack_data = None        # 完整8字节ACK数据
        self._bl_cts_event = threading.Event()
        self._bl_cts_next_pkt = 0
        self._bl_endack_event = threading.Event()
        self._bl_tp_seq = 0             # TP_DT 序号

        # 统计计数
        self._tx_bytes = 0
        self._rx_bytes = 0
        self._can_rx_count = 0
        self._can_tx_count = 0
        self._connect_time = None
        self._filter_ids = set()  # CAN ID 过滤集合, 空=显示全部

        # 发送历史
        self._send_history = []
        self._history_index = -1

        self._build_ui()
        self._refresh_ports()
        self._bind_shortcuts()
        self._update_stats_timer()

        # 初始化日志文件 (写入标题)
        with open(LOG_FILE_PATH, "a", encoding="utf-8") as f:
            f.write(f"\n{'='*60}\n")
            f.write(f"[{self._now()}] USB_Tool 启动\n")
            f.write(f"{'='*60}\n")

    @staticmethod
    def _now() -> str:
        return datetime.datetime.now().strftime("%H:%M:%S.%f")[:-3]

    _log_line_count = 0
    _LOG_MAX_LINES = 1024

    def _write_log(self, line: str):
        """追加一行到实时日志文件, 超过1024行时截断"""
        try:
            self._log_line_count += 1
            if self._log_line_count % 256 == 0:
                self._truncate_log()
            with open(LOG_FILE_PATH, "a", encoding="utf-8") as f:
                f.write(f"[{self._now()}] {line}\n")
        except OSError:
            pass

    def _truncate_log(self):
        """截断日志文件, 只保留最近1024行"""
        try:
            if not os.path.exists(LOG_FILE_PATH):
                return
            with open(LOG_FILE_PATH, "r", encoding="utf-8") as f:
                lines = f.readlines()
            if len(lines) > self._LOG_MAX_LINES:
                with open(LOG_FILE_PATH, "w", encoding="utf-8") as f:
                    f.writelines(lines[-self._LOG_MAX_LINES:])
                self._log_line_count = self._LOG_MAX_LINES
        except OSError:
            pass

    # ================== 快捷键绑定 ==================
    def _bind_shortcuts(self):
        self.root.bind("<Control-Return>", lambda e: self._send_bb_frame())
        self.root.bind("<Control-l>", lambda e: self._clear_rx())
        self.root.bind("<Control-L>", lambda e: self._clear_rx())
        self.root.bind("<Control-e>", lambda e: self._export_log())
        self.root.bind("<Control-E>", lambda e: self._export_log())
        self.root.bind("<F5>", lambda e: self._refresh_ports())

    # ================== 深色主题 ==================
    def _apply_theme(self):
        style = ttk.Style()
        style.theme_use("clam")
        style.configure(".", background=BG, foreground=FG, fieldbackground=BG_DARK,
                         bordercolor=BORDER, troughcolor=BG_WIDGET, selectbackground=BG_HOVER,
                         selectforeground=FG, font=("Microsoft YaHei UI", 9))
        style.configure("TFrame", background=BG)
        style.configure("TLabel", background=BG, foreground=FG)
        style.configure("TButton", background=BG_WIDGET, foreground=FG, padding=(8, 4),
                         borderwidth=1, relief="flat")
        style.map("TButton", background=[("active", BG_HOVER), ("pressed", ACCENT)],
                   foreground=[("disabled", FG_Muted)])
        style.configure("TEntry", fieldbackground=BG_DARK, foreground=FG, insertcolor=FG,
                         borderwidth=1, relief="flat")
        style.map("TEntry", fieldbackground=[("focus", BG_WIDGET)], bordercolor=[("focus", ACCENT)])
        style.configure("TCombobox", fieldbackground=BG_DARK, background=BG_WIDGET,
                         foreground=FG, arrowcolor=FG, borderwidth=1)
        style.map("TCombobox", fieldbackground=[("readonly", BG_DARK), ("focus", BG_WIDGET)],
                   foreground=[("readonly", FG)])
        self.root.option_add("*TCombobox*Listbox.background", BG_DARK)
        self.root.option_add("*TCombobox*Listbox.foreground", FG)
        self.root.option_add("*TCombobox*Listbox.selectBackground", BG_HOVER)
        self.root.option_add("*TCombobox*Listbox.selectForeground", FG)
        style.configure("TLabelframe", background=BG, foreground=ACCENT, bordercolor=BORDER)
        style.configure("TLabelframe.Label", background=BG, foreground=ACCENT, font=("Microsoft YaHei UI", 9, "bold"))
        style.configure("TNotebook", background=BG, borderwidth=0)
        style.configure("TNotebook.Tab", background=BG_WIDGET, foreground=FG, padding=(12, 4))
        style.map("TNotebook.Tab", background=[("selected", BG), ("active", BG_HOVER)],
                   foreground=[("selected", ACCENT)])
        style.configure("Horizontal.TProgressbar", background=ACCENT, troughcolor=BG_WIDGET,
                         borderwidth=0, lightcolor=ACCENT, darkcolor=ACCENT)
        style.configure("TCheckbutton", background=BG, foreground=FG)
        style.map("TCheckbutton", background=[("active", BG)])
        style.configure("TSeparator", background=BORDER)
        style.configure("Status.TLabel", background=BG_DARK, foreground=FG_DIM, padding=(6, 3),
                         borderwidth=1, relief="flat")

    # ================== UI 构建 ==================
    def _build_ui(self):
        self._apply_theme()
        self.root.configure(bg=BG)
        # ---- 顶部: 串口设置 ----
        top = ttk.Frame(self.root, padding=5)
        top.pack(fill=tk.X)

        ttk.Label(top, text="串口:").pack(side=tk.LEFT)
        self.port_var = tk.StringVar()
        self.port_cb = ttk.Combobox(top, textvariable=self.port_var, width=10, state="readonly")
        self.port_cb.pack(side=tk.LEFT, padx=2)

        ttk.Label(top, text="波特率:").pack(side=tk.LEFT, padx=(10,0))
        self.baud_var = tk.StringVar(value="115200")
        self.baud_cb = ttk.Combobox(top, textvariable=self.baud_var, width=8,
                                     values=["9600","19200","38400","57600","115200","230400","460800","921600"])
        self.baud_cb.pack(side=tk.LEFT, padx=2)

        self.connect_btn = ttk.Button(top, text="连接", command=self._toggle_connect, width=6)
        self.connect_btn.pack(side=tk.LEFT, padx=5)

        self.refresh_btn = ttk.Button(top, text="刷新(F5)", command=self._refresh_ports, width=8)
        self.refresh_btn.pack(side=tk.LEFT)

        # 模式状态指示
        ttk.Label(top, text="    当前模式:").pack(side=tk.LEFT, padx=(20, 0))
        self.mode_label = ttk.Label(top, text="未知", font=("Consolas", 10, "bold"))
        self.mode_label.pack(side=tk.LEFT, padx=5)
        self.mode_indicator = tk.Canvas(top, width=14, height=14, highlightthickness=0, bg=BG)
        self.mode_indicator.pack(side=tk.LEFT)
        self._set_mode_indicator(FG_Muted)

        ttk.Separator(self.root, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=3)

        # ---- Tab 页 ----
        notebook = ttk.Notebook(self.root)
        notebook.pack(fill=tk.BOTH, expand=True, padx=5, pady=(0,5))

        # Tab 1: CAN 工具
        can_tab = ttk.Frame(notebook)
        notebook.add(can_tab, text="CAN 工具")
        self._build_can_tab(can_tab)

        # Tab 2: Bootloader
        boot_tab = ttk.Frame(notebook)
        notebook.add(boot_tab, text="Bootloader")
        self._build_boot_tab(boot_tab)

        # ---- 底部状态栏 ----
        status_frame = ttk.Frame(self.root)
        status_frame.pack(fill=tk.X, side=tk.BOTTOM)
        self.status_var = tk.StringVar(value="未连接")
        ttk.Label(status_frame, textvariable=self.status_var, style="Status.TLabel", anchor=tk.W).pack(side=tk.LEFT, fill=tk.X, expand=True)
        self.stats_var = tk.StringVar(value="TX: 0 | RX: 0 | CAN: 0")
        ttk.Label(status_frame, textvariable=self.stats_var, style="Status.TLabel", width=30).pack(side=tk.RIGHT)

    # ================== CAN 工具 Tab ==================
    def _build_can_tab(self, parent):
        # 模式切换 + CAN ID
        mid = ttk.Frame(parent, padding=5)
        mid.pack(fill=tk.X)

        mode_frame = ttk.LabelFrame(mid, text="手动模式切换", padding=5)
        mode_frame.pack(side=tk.LEFT, fill=tk.Y, padx=(0,5))
        ttk.Button(mode_frame, text="AA 标准ID+65B桥\n(AA AA AA)", width=16,
                   command=lambda: self._send_raw(MODE_LEGACY)).pack(pady=2)
        ttk.Button(mode_frame, text="BB 扩展ID+8B直发\n(BB BB BB)", width=16,
                   command=lambda: self._send_raw(MODE_PROTOCOL_BRIDGE)).pack(pady=2)
        ttk.Label(mode_frame, text="(收到CAN帧自动切换)", font=("", 8)).pack()

        cid_frame = ttk.LabelFrame(mid, text="CAN ID 控制 (A5 A5 ...)", padding=5)
        cid_frame.pack(side=tk.LEFT, fill=tk.Y, padx=5)
        row1 = ttk.Frame(cid_frame); row1.pack(fill=tk.X, pady=1)
        ttk.Button(row1, text="读 CAN ID", width=10,
                   command=lambda: self._send_ctrl_frame(0x00, 0x0000)).pack(side=tk.LEFT, padx=2)
        row2 = ttk.Frame(cid_frame); row2.pack(fill=tk.X, pady=1)
        ttk.Label(row2, text="写 CAN ID:").pack(side=tk.LEFT)
        self.id_entry = ttk.Entry(row2, width=6); self.id_entry.pack(side=tk.LEFT, padx=2)
        self.id_entry.insert(0, "0x781")
        ttk.Button(row2, text="写入", width=6, command=self._write_can_id).pack(side=tk.LEFT, padx=2)
        row3 = ttk.Frame(cid_frame); row3.pack(fill=tk.X, pady=1)
        ttk.Button(row3, text="软件重启 (CMD=03)", width=16,
                   command=lambda: self._send_ctrl_frame(0x03, 0x0000)).pack(side=tk.LEFT, padx=2)

        # 过滤设置
        filter_frame = ttk.LabelFrame(mid, text="帧过滤", padding=5)
        filter_frame.pack(side=tk.LEFT, fill=tk.Y, padx=5)
        ttk.Label(filter_frame, text="显示ID(空=全部):").pack(anchor=tk.W)
        self.filter_entry = ttk.Entry(filter_frame, width=15)
        self.filter_entry.pack(fill=tk.X, pady=2)
        ttk.Button(filter_frame, text="应用过滤", width=10, command=self._apply_filter).pack(pady=2)
        ttk.Button(filter_frame, text="清除过滤", width=10, command=self._clear_filter).pack(pady=2)

        ttk.Separator(parent, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=3)

        # BB 扩展ID发送 + 历史
        bb_frame = ttk.LabelFrame(parent, text="BB 模式: 扩展ID + 8字节单帧直发 (Ctrl+Enter 发送)", padding=5)
        bb_frame.pack(fill=tk.X, padx=5)
        bb_row = ttk.Frame(bb_frame); bb_row.pack(fill=tk.X)
        ttk.Label(bb_row, text="扩展ID:").pack(side=tk.LEFT)
        self.bb_ext_id = ttk.Entry(bb_row, width=12); self.bb_ext_id.pack(side=tk.LEFT, padx=2)
        self.bb_ext_id.insert(0, "0x18FFA0E2")
        ttk.Label(bb_row, text="数据(HEX):").pack(side=tk.LEFT, padx=(10, 0))
        self.bb_data = ttk.Entry(bb_row, width=30); self.bb_data.pack(side=tk.LEFT, padx=2)
        self.bb_data.insert(0, "DE AD BE EF CA FE BA BE")
        ttk.Button(bb_row, text="发送", command=self._send_bb_frame).pack(side=tk.LEFT, padx=5)

        # 发送历史
        hist_row = ttk.Frame(bb_frame); hist_row.pack(fill=tk.X, pady=(3,0))
        ttk.Label(hist_row, text="历史:").pack(side=tk.LEFT)
        self.history_cb = ttk.Combobox(hist_row, width=40, state="readonly")
        self.history_cb.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=2)
        self.history_cb.bind("<<ComboboxSelected>>", self._load_history)

        ttk.Separator(parent, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=3)

        # AA 协议帧
        proto_frame = ttk.LabelFrame(parent, text="AA 模式: 协议转化帧 (65字节)", padding=5)
        proto_frame.pack(fill=tk.X, padx=5)
        rowp = ttk.Frame(proto_frame); rowp.pack(fill=tk.X, pady=2)
        ttk.Label(rowp, text="首字节:").pack(side=tk.LEFT)
        self.proto_header = ttk.Combobox(rowp, width=6, values=["0x40","0x23","0xFD","0xFE","0x41"], state="readonly")
        self.proto_header.pack(side=tk.LEFT, padx=2); self.proto_header.set("0x40")
        ttk.Label(rowp, text="填充值:").pack(side=tk.LEFT, padx=(10,0))
        self.proto_fill = ttk.Entry(rowp, width=4); self.proto_fill.pack(side=tk.LEFT, padx=2)
        self.proto_fill.insert(0, "00")
        ttk.Button(rowp, text="发送 65 字节帧", command=self._send_protocol_frame).pack(side=tk.LEFT, padx=5)

        ttk.Separator(parent, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=3)

        # 接收区
        rx_frame = ttk.LabelFrame(parent, text="接收数据", padding=5)
        rx_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=(0,5))
        rx_toolbar = ttk.Frame(rx_frame); rx_toolbar.pack(fill=tk.X)
        self.hex_display = tk.BooleanVar(value=True)
        ttk.Checkbutton(rx_toolbar, text="HEX 显示", variable=self.hex_display).pack(side=tk.LEFT)
        self.ts_display = tk.BooleanVar(value=True)
        ttk.Checkbutton(rx_toolbar, text="时间戳", variable=self.ts_display).pack(side=tk.LEFT, padx=10)
        ttk.Button(rx_toolbar, text="导出(Ctrl+E)", command=self._export_log, width=10).pack(side=tk.LEFT, padx=5)
        ttk.Button(rx_toolbar, text="清空(Ctrl+L)", command=self._clear_rx, width=10).pack(side=tk.RIGHT)
        self.rx_count_label = ttk.Label(rx_toolbar, text="接收: 0 字节")
        self.rx_count_label.pack(side=tk.RIGHT, padx=5)
        self.can_rx_count_label = ttk.Label(rx_toolbar, text="CAN帧: 0")
        self.can_rx_count_label.pack(side=tk.RIGHT, padx=5)
        self.rx_text = scrolledtext.ScrolledText(rx_frame, height=12, font=("Consolas", 10),
                                                 bg=BG_DARK, fg=FG, insertbackground=FG,
                                                 selectbackground=BG_HOVER, selectforeground=FG,
                                                 relief="flat", borderwidth=0)
        self.rx_text.pack(fill=tk.BOTH, expand=True)
        self.rx_text.tag_configure("tx", foreground=ACCENT)
        self.rx_text.tag_configure("rx", foreground=ORANGE)
        self.rx_text.tag_configure("ctrl", foreground=GREEN)

    # ================== Bootloader Tab ==================
    def _build_boot_tab(self, parent):
        # 固件文件 + 按钮 一行
        top_frame = ttk.Frame(parent, padding=5)
        top_frame.pack(fill=tk.X)

        ttk.Label(top_frame, text="固件:").pack(side=tk.LEFT)
        self.bl_fw_path = ttk.Entry(top_frame, width=40)
        self.bl_fw_path.pack(side=tk.LEFT, padx=2, fill=tk.X, expand=True)
        ttk.Button(top_frame, text="选择文件", command=self._bl_browse_fw, width=10).pack(side=tk.LEFT, padx=2)
        ttk.Button(top_frame, text="开始升级", command=self._bl_one_click, width=10).pack(side=tk.LEFT, padx=2)
        self.bl_stop_btn = ttk.Button(top_frame, text="停止升级", command=self._bl_stop_upgrade, width=10)
        self.bl_stop_btn.pack(side=tk.LEFT, padx=2)
        ttk.Button(top_frame, text="清除日志", command=self._bl_clear_log, width=10).pack(side=tk.LEFT, padx=2)

        # Boot B 操作
        boot_b_frame = ttk.Frame(parent, padding=(5, 0))
        boot_b_frame.pack(fill=tk.X)
        ttk.Button(boot_b_frame, text="升级 Boot B", command=self._bl_update_boot_b, width=12).pack(side=tk.LEFT, padx=2)
        ttk.Button(boot_b_frame, text="跳转 Boot B", command=self._bl_jump_boot_b, width=12).pack(side=tk.LEFT, padx=2)

        # 固件信息 + 配置
        info_frame = ttk.Frame(parent, padding=(5, 0))
        info_frame.pack(fill=tk.X)
        self.bl_fw_info = ttk.Label(info_frame, text="未加载固件", font=("", 9))
        self.bl_fw_info.pack(side=tk.LEFT)
        self.bl_checksum_label = ttk.Label(info_frame, text="", font=("Consolas", 9))
        self.bl_checksum_label.pack(side=tk.LEFT, padx=20)
        ttk.Label(info_frame, text="PC地址:").pack(side=tk.RIGHT, padx=(10, 0))
        self.bl_pc_addr = ttk.Entry(info_frame, width=6)
        self.bl_pc_addr.pack(side=tk.RIGHT)
        self.bl_pc_addr.insert(0, "0xFF")
        ttk.Label(info_frame, text="设备地址:").pack(side=tk.RIGHT, padx=(10, 0))
        self.bl_dev_addr = ttk.Entry(info_frame, width=6)
        self.bl_dev_addr.pack(side=tk.RIGHT)
        self.bl_dev_addr.insert(0, "0x01")

        # 进度条 (平滑动画)
        prog_frame = ttk.Frame(parent, padding=5)
        prog_frame.pack(fill=tk.X)
        self.bl_progress = ttk.Progressbar(prog_frame, orient=tk.HORIZONTAL, mode='determinate')
        self.bl_progress.pack(fill=tk.X)
        self.bl_progress_label = ttk.Label(prog_frame, text="就绪", font=("", 9))
        self.bl_progress_label.pack(anchor=tk.W)
        self._bl_progress_target = 0.0   # 后台线程设置的目标进度
        self._bl_progress_display = 0.0  # UI 当前显示的进度
        self._bl_progress_text = "就绪"
        self._bl_animating = False

        # 日志区
        log_frame = ttk.LabelFrame(parent, text="日志", padding=5)
        log_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=(0, 5))
        self.bl_log = scrolledtext.ScrolledText(log_frame, height=18, font=("Consolas", 10),
                                                bg=BG_DARK, fg=FG, insertbackground=FG,
                                                selectbackground=BG_HOVER, selectforeground=FG,
                                                relief="flat", borderwidth=0)
        self.bl_log.pack(fill=tk.BOTH, expand=True)

    # ================== 模式指示 ==================
    def _set_mode_indicator(self, color: str):
        self.mode_indicator.delete("all")
        self.mode_indicator.create_oval(2, 2, 12, 12, fill=color, outline="")

    def _update_mode_display(self, mode_name: str, color: str):
        self.mode_label.config(text=mode_name)
        self._set_mode_indicator(color)

    # ================== 串口操作 ==================
    def _refresh_ports(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.port_cb["values"] = ports
        if ports and not self.port_var.get():
            stlink = [p for p in ports if "STLink" in p or "STMicro" in p]
            self.port_var.set(stlink[0] if stlink else ports[0])

    def _toggle_connect(self):
        if self.ser and self.ser.is_open:
            self._disconnect()
        else:
            self._connect()

    def _connect(self):
        port = self.port_var.get()
        if not port:
            messagebox.showwarning("警告", "请选择串口")
            return
        try:
            baud = int(self.baud_var.get())
            self.ser = serial.Serial(port, baud, timeout=0.05)
            self.running = True
            self._connect_time = time.time()
            self.read_thread = threading.Thread(target=self._read_loop, daemon=True)
            self.read_thread.start()
            self.connect_btn.config(text="断开")
            self.status_var.set(f"已连接 {port} @ {baud}")
        except (ValueError, serial.SerialException) as e:
            messagebox.showerror("连接失败", str(e))

    def _disconnect(self):
        self.running = False
        if self.read_thread:
            self.read_thread.join(timeout=1)
        if self.ser:
            self.ser.close()
            self.ser = None
        self._connect_time = None
        self.connect_btn.config(text="连接")
        self.status_var.set("未连接")

    def _send_raw(self, data: bytes):
        if not self.ser or not self.ser.is_open:
            return False
        try:
            self.ser.write(data)
            self._tx_bytes += len(data)
            self._log_tx(data)
            return True
        except (serial.SerialException, OSError):
            return False

    def _send_ctrl_frame(self, cmd: int, param: int):
        data = FRAME_CTRL_MAGIC + bytes([cmd, (param >> 8) & 0xFF, param & 0xFF])
        self._send_raw(data)

    def _write_can_id(self):
        try:
            cid = int(self.id_entry.get(), 16)
            if 0x001 <= cid <= 0x7FF:
                self._send_ctrl_frame(0x01, cid)
            else:
                messagebox.showwarning("警告", "CAN ID 范围: 0x001 ~ 0x7FF")
        except ValueError:
            messagebox.showwarning("警告", "无效的 CAN ID 格式")

    def _send_protocol_frame(self):
        try:
            header = int(self.proto_header.get(), 16)
            fill = int(self.proto_fill.get(), 16) & 0xFF
        except ValueError:
            messagebox.showwarning("警告", "无效的十六进制值")
            return
        data = bytes([header]) + bytes([fill] * 64)
        self._send_raw(data)

    def _send_bb_frame(self):
        try:
            ext_id = int(self.bb_ext_id.get(), 16) & 0x1FFFFFFF
            hex_str = self.bb_data.get().replace(" ", "").replace(",", "")
            data = bytes.fromhex(hex_str)
            data = data[:8].ljust(8, b'\x00')
            frame = bytes([(ext_id >> 24) & 0xFF, (ext_id >> 16) & 0xFF,
                           (ext_id >> 8) & 0xFF, ext_id & 0xFF]) + data
            self._send_raw(frame)
            self._add_to_history(f"0x{ext_id:08X} {self.bb_data.get()}")
        except (ValueError, IndexError):
            messagebox.showwarning("警告", "无效的扩展ID或HEX数据")

    def _add_to_history(self, entry: str):
        """添加到发送历史"""
        if entry in self._send_history:
            self._send_history.remove(entry)
        self._send_history.insert(0, entry)
        self._send_history = self._send_history[:20]  # 保留最近20条
        self.history_cb["values"] = self._send_history

    def _load_history(self, event=None):
        """从历史加载"""
        sel = self.history_cb.get()
        if sel:
            parts = sel.split(" ", 1)
            if len(parts) == 2:
                self.bb_ext_id.delete(0, tk.END)
                self.bb_ext_id.insert(0, parts[0])
                self.bb_data.delete(0, tk.END)
                self.bb_data.insert(0, parts[1])

    def _clear_rx(self):
        self.rx_text.delete("1.0", tk.END)
        self._rx_bytes = 0
        self._can_rx_count = 0
        self.rx_count_label.config(text="接收: 0 字节")
        self.can_rx_count_label.config(text="CAN帧: 0")

    def _export_log(self):
        """导出接收区日志"""
        path = filedialog.asksaveasfilename(
            title="导出日志",
            defaultextension=".txt",
            filetypes=[("文本文件", "*.txt"), ("所有文件", "*.*")]
        )
        if path:
            try:
                content = self.rx_text.get("1.0", tk.END)
                with open(path, "w", encoding="utf-8") as f:
                    f.write(content)
                messagebox.showinfo("导出成功", f"已保存到 {path}")
            except OSError as e:
                messagebox.showerror("导出失败", str(e))

    # ================== 帧过滤 ==================
    def _apply_filter(self):
        """应用 CAN ID 过滤"""
        text = self.filter_entry.get().strip()
        if not text:
            self._filter_ids.clear()
            return
        try:
            ids = []
            for part in text.replace(",", " ").split():
                ids.append(int(part, 16))
            self._filter_ids = set(ids)
        except ValueError:
            messagebox.showwarning("警告", "无效的 CAN ID 格式, 使用逗号或空格分隔")

    def _clear_filter(self):
        """清除过滤"""
        self._filter_ids.clear()
        self.filter_entry.delete(0, tk.END)

    # ================== 收发日志 ==================
    def _log_tx(self, data: bytes):
        # 12字节CAN帧: 解析后与RX格式一致
        if len(data) == CAN_RX_FRAME_LEN:
            raw_id = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]
            if raw_id <= 0x1FFFFFFF:
                self._can_tx_count += 1
                is_ext = (raw_id > 0x7FF)
                frame_type = "EXT" if is_ext else "STD"
                id_str = f"0x{raw_id:08X}" if is_ext else f"0x{raw_id:03X}"
                payload_hex = " ".join(f"{b:02X}" for b in data[4:12])
                line = f">>> CAN TX [{frame_type}] ID={id_str}  Data: {payload_hex}  (#{self._can_tx_count})"
                self.rx_text.insert(tk.END, line + "\n", "tx")
                self.rx_text.see(tk.END)
                self._write_log(line)
                return
        # 非CAN帧数据: 原始HEX格式
        self._can_tx_count += 1
        hx = " ".join(f"{b:02X}" for b in data)
        line = f">>> TX [{len(data)}B]: {hx}"
        self.rx_text.insert(tk.END, line + "\n", "tx")
        self.rx_text.see(tk.END)
        self._write_log(line)

    # 控制帧响应命令名映射
    _CTRL_CMD_NAMES = {
        0x00: "读CAN ID",
        0x01: "写CAN ID",
        0x81: "写CAN ID失败",
        0x03: "软件重启",
    }

    def _try_parse_can_frame(self, data: bytes) -> bool:
        # 解析 5 字节控制帧响应: A5 A5 [cmd] [hi] [lo]
        if len(data) == FRAME_CTRL_LEN and data[0] == 0xA5 and data[1] == 0xA5:
            cmd = data[2]
            param = (data[3] << 8) | data[4]
            cmd_name = self._CTRL_CMD_NAMES.get(cmd, f"未知(0x{cmd:02X})")
            hx = " ".join(f"{b:02X}" for b in data)
            line = f"<<< 控制响应: {cmd_name}  CAN ID=0x{param:03X}  [{hx}]"
            self._write_log(line)
            self._append_log(line, "ctrl")
            return True

        # 解析 12 字节 CAN 帧
        if len(data) != CAN_RX_FRAME_LEN:
            return False

        raw_id = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]
        if raw_id > 0x1FFFFFFF:
            return False

        # 过滤检查
        if self._filter_ids and raw_id not in self._filter_ids:
            return True  # 静默丢弃, 但仍计数

        is_extended = (raw_id > 0x7FF)
        if is_extended:
            self.root.after(0, lambda: self._update_mode_display("BB (扩展ID)", GREEN))
            frame_type = "EXT"
            id_str = f"0x{raw_id:08X}"
        else:
            self.root.after(0, lambda: self._update_mode_display("AA (标准ID)", ACCENT))
            frame_type = "STD"
            id_str = f"0x{raw_id:03X}"

        payload = data[4:12]
        payload_hex = " ".join(f"{b:02X}" for b in payload)
        self._can_rx_count += 1

        # 分发给 bootloader 回调 (异常不能阻止数据显示)
        if is_extended:
            try:
                self._bl_on_can_rx(raw_id, payload)
            except Exception:
                pass

        line = f"<<< CAN RX [{frame_type}] ID={id_str}  Data: {payload_hex}  (#{self._can_rx_count})"
        self._write_log(line)
        self._append_log(line, "rx")
        return True

    def _append_log(self, line: str, tag: str = "rx"):
        try:
            self.rx_text.insert(tk.END, line + "\n", tag)
            self.rx_text.see(tk.END)
        except Exception:
            self._write_log(f"[ERROR] _append_log insert失败: {traceback.format_exc()}")
        self.can_rx_count_label.config(text=f"CAN帧: {self._can_rx_count}")

    def _log_rx(self, data: bytes):
        # 过滤全零填充帧 (缓冲区对齐产生的无效数据)
        if len(data) == CAN_RX_FRAME_LEN and data == b'\x00' * CAN_RX_FRAME_LEN:
            self._rx_bytes += len(data)
            return
        # 12字节CAN帧中, TP传输帧(TP.DT=0xEB, TP.CM=0xEC)不显示, 其余帧正常显示
        if len(data) == CAN_RX_FRAME_LEN:
            raw_id = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]
            pf = (raw_id >> 16) & 0xFF
            if raw_id <= 0x1FFFFFFF and pf in (PF_TP_DT, PF_TP_CM):
                self._rx_bytes += len(data)
                self._can_rx_count += 1
                self.rx_count_label.config(text=f"接收: {self._rx_bytes} 字节")
                return
        self._rx_bytes += len(data)
        self.rx_count_label.config(text=f"接收: {self._rx_bytes} 字节")
        if self._try_parse_can_frame(data):
            return
        # 非CAN帧数据
        hx = " ".join(f"{b:02X}" for b in data)
        line = f"<<< RX [{len(data)}B]: {hx}"
        self._write_log(line)
        try:
            self.rx_text.insert(tk.END, line + "\n", "rx")
            self.rx_text.see(tk.END)
        except Exception:
            self._write_log(f"[ERROR] _log_rx insert失败: {traceback.format_exc()}")

    # ================== 统计更新 ==================
    def _update_stats_timer(self):
        """定时更新状态栏统计"""
        elapsed = ""
        if self._connect_time:
            secs = int(time.time() - self._connect_time)
            elapsed = f" | {secs//3600:02d}:{(secs%3600)//60:02d}:{secs%60:02d}"
        self.stats_var.set(f"TX: {self._tx_bytes}B | RX: {self._rx_bytes}B | CAN: {self._can_rx_count}{elapsed}")
        self.root.after(1000, self._update_stats_timer)

    # ================== 接收线程 ==================
    def _read_loop(self):
        buf = bytearray()
        while self.running:
            try:
                if self.ser and self.ser.is_open and self.ser.in_waiting:
                    chunk = self.ser.read(self.ser.in_waiting)
                    buf.extend(chunk)
                else:
                    time.sleep(0.0001)

                # 解析: 优先检测 A5 A5 控制帧(5字节), 再处理12字节CAN帧
                offset = 0
                while offset < len(buf):
                    remaining = len(buf) - offset

                    # 检测 5 字节控制帧: A5 A5 [cmd] [hi] [lo]
                    if remaining >= FRAME_CTRL_LEN and buf[offset] == 0xA5 and buf[offset + 1] == 0xA5:
                        frame_data = bytes(buf[offset:offset + FRAME_CTRL_LEN])
                        if self._bl_running:
                            self._bl_direct_parse(frame_data)
                        else:
                            self.root.after(0, lambda d=frame_data: self._log_rx(d))
                        offset += FRAME_CTRL_LEN
                        continue

                    # 12 字节 CAN 帧
                    if remaining >= CAN_RX_FRAME_LEN:
                        frame_data = bytes(buf[offset:offset + CAN_RX_FRAME_LEN])
                        if self._bl_running:
                            self._bl_direct_parse(frame_data)
                        else:
                            self.root.after(0, lambda d=frame_data: self._log_rx(d))
                        offset += CAN_RX_FRAME_LEN
                        continue

                    break  # 数据不够一帧, 等待更多数据

                if offset > 0:
                    buf = buf[offset:]

                # 超时丢弃：缓冲区有不完整碎片且超过1秒无新数据
                if len(buf) > 0:
                    if not hasattr(self, '_partial_tick') or self._partial_tick is None:
                        self._partial_tick = time.time()
                    elif time.time() - self._partial_tick > 1.0:
                        # 显示丢弃的数据到UI，便于排查
                        hx = buf.hex(" ").upper()
                        self._write_log(f"[WARN] 丢弃不完整数据 {len(buf)}B: {hx}")
                        self.root.after(0, lambda h=hx: self.rx_text.insert(tk.END,
                            f"[{self._now()}] <<< 丢弃碎片 [{len(buf)}B]: {hx}\n", "rx"))
                        self.root.after(0, self.rx_text.see, tk.END)
                        buf.clear()
                        self._partial_tick = None
                else:
                    self._partial_tick = None

            except (serial.SerialException, OSError):
                self.root.after(0, self._disconnect)
                break
            except Exception:
                err_msg = traceback.format_exc()
                self._write_log(f"[ERROR] 读线程异常: {err_msg}")
                self.root.after(0, lambda msg=err_msg: self.rx_text.insert(tk.END,
                    f"[{self._now()}] <<< [ERROR] 读线程异常:\n{msg}\n", "rx"))
                self.root.after(0, self.rx_text.see, tk.END)
                time.sleep(0.1)

    # ================================================================
    #                     Bootloader 功能
    # ================================================================

    def _bl_log(self, msg: str):
        """Bootloader 日志输出"""
        self.bl_log.insert(tk.END, msg + "\n")
        self.bl_log.see(tk.END)
        self._write_log(msg)

    def _bl_clear_log(self):
        self.bl_log.delete("1.0", tk.END)

    def _bl_stop_upgrade(self):
        if self._bl_running:
            self._bl_stop = True
            self._bl_log("[停止] 用户请求停止升级...")

    def _bl_set_progress(self, value: float, text: str = ""):
        """设置目标进度值, 由动画定时器平滑趋近"""
        self._bl_progress_target = value
        if text:
            self._bl_progress_text = text
        if not self._bl_animating:
            self._bl_animating = True
            self._bl_progress_display = self.bl_progress["value"]
            self._bl_progress_tick()

    def _bl_progress_tick(self):
        """定时器驱动的平滑进度动画 (~30fps, 指数插值趋近目标)"""
        target = self._bl_progress_target
        cur = self._bl_progress_display
        diff = target - cur
        if abs(diff) < 0.3:
            # 足够接近, 直接对齐
            self._bl_progress_display = target
            self.bl_progress["value"] = target
            self.bl_progress_label.config(text=f"{int(target)}%  {self._bl_progress_text}")
            if target >= 100 or (target == 0 and self._bl_progress_text in ("已停止", "就绪")):
                self._bl_animating = False
                return
        else:
            # 每帧追赶差值的25%, 产生平滑减速效果
            step = diff * 0.25
            if abs(step) < 0.5:
                step = 0.5 if diff > 0 else -0.5
            self._bl_progress_display += step
            self.bl_progress["value"] = self._bl_progress_display
            self.bl_progress_label.config(text=f"{int(self._bl_progress_display)}%  {self._bl_progress_text}")
        self.root.after(33, self._bl_progress_tick)

    def _bl_get_dev_addr(self) -> int:
        try:
            return int(self.bl_dev_addr.get(), 16) & 0xFF
        except ValueError:
            return BL_ADDR_DEVICE

    def _bl_get_pc_addr(self) -> int:
        try:
            return int(self.bl_pc_addr.get(), 16) & 0xFF
        except (ValueError, AttributeError):
            return BL_ADDR_PC

    # ---- Bootloader 前导码 ----

    def _bl_send_preamble(self) -> bool:
        """发送升级前导码: 标准CAN帧, ID=0x780+设备地址, 65字节 (与CAN_OTA协议一致)"""
        dev = self._bl_get_dev_addr()
        can_id = 0x780 + dev
        payload = bytes([0x23, 0x00, 0x45]) + b'\x00' * 61 + bytes([0xCA])
        for offset in range(0, len(payload), 8):
            chunk = payload[offset:offset + 8].ljust(8, b'\x00')
            frame = bytes([(can_id >> 24) & 0xFF, (can_id >> 16) & 0xFF,
                           (can_id >> 8) & 0xFF, can_id & 0xFF]) + chunk
            if not self._send_raw(frame):
                return False
        return True

    # ---- Bootloader CAN 收发 ----

    def _bl_send_can(self, pf: int, ps: int, sa: int, data: bytes, priority: int = PRIORITY_BOOT_CMD):
        """通过12字节格式发送一帧J1939 CAN消息"""
        can_id = j1939_build_id(priority, pf, ps, sa)
        frame = build_can_frame(can_id, data)
        return self._send_raw(frame)

    def _bl_on_can_rx(self, can_id: int, data: bytes):
        """CAN 接收回调: 解析J1939帧并分发给bootloader等待逻辑"""
        jid = j1939_parse_id(can_id)

        # 仅处理发给PC或广播的消息
        if jid["ps"] != self._bl_get_pc_addr() and jid["ps"] != BL_ADDR_BROADCAST:
            return

        if jid["pf"] == PF_ACK:
            ack_type = data[0]
            req_pgn = data[5] | (data[6] << 8)
            self._bl_ack_result = (ack_type, req_pgn)
            self._bl_ack_data = data
            self._bl_ack_event.set()
            self._bl_rx_queue.put({"pf": PF_ACK, "d0": data[0], "pgn": req_pgn})

        elif jid["pf"] == PF_TP_CM:
            ctrl = data[0]
            if ctrl == TP_CM_CTS:
                self._bl_cts_next_pkt = data[2]
                self._bl_cts_event.set()
            elif ctrl == TP_CM_END_ACK:
                self._bl_endack_event.set()
            self._bl_rx_queue.put({"pf": PF_TP_CM, "d0": data[0]})

        elif jid["pf"] == PF_HEARTBEAT:
            dev_id = data[0]
            bl_state = data[1]
            be_iap = data[2]
            self._bl_log(f"[心跳] 设备=0x{dev_id:02X} 状态={BL_STATE_NAMES.get(bl_state, str(bl_state))} IAP={be_iap}")

    def _bl_direct_parse(self, data: bytes):
        """传输期间直接解析CAN帧, 不经过UI线程, 避免事件队列拥堵"""
        # 5字节控制帧
        if len(data) == FRAME_CTRL_LEN and data[0] == 0xA5 and data[1] == 0xA5:
            self._rx_bytes += len(data)
            return

        # 12字节CAN帧
        if len(data) != CAN_RX_FRAME_LEN:
            return

        raw_id = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3]
        if raw_id > 0x1FFFFFFF:
            return

        # 计数由 _log_rx 统一更新, 此处不重复累加

        is_extended = (raw_id > 0x7FF)
        if is_extended:
            payload = data[4:12]
            try:
                self._bl_on_can_rx(raw_id, payload)
            except Exception:
                pass

        # 非数据帧显示到CAN窗口 (TP.DT由_log_rx内部过滤)
        self.root.after(0, lambda d=data: self._log_rx(d))

    def _bl_clear_events(self):
        """发送前清空队列和事件, 防止旧数据残留"""
        self._bl_ack_event.clear()
        self._bl_ack_result = None
        self._bl_ack_data = None
        self._bl_cts_event.clear()
        self._bl_endack_event.clear()
        while not self._bl_rx_queue.empty():
            try:
                self._bl_rx_queue.get_nowait()
            except queue.Empty:
                break

    def _bl_wait_ack(self, timeout: float = 2.0) -> tuple:
        """队列轮询等待ACK, 返回 (ack_type, pgn) 或 None"""
        start = time.time()
        while (time.time() - start) < timeout:
            if self._bl_stop:
                return None
            try:
                item = self._bl_rx_queue.get(timeout=0.0002)
                if item["pf"] == PF_ACK:
                    return (item["d0"], item.get("pgn", 0))
            except queue.Empty:
                continue
        return None

    def _bl_wait_cts(self, timeout: float = 2.0) -> bool:
        """队列轮询等待CTS"""
        start = time.time()
        while (time.time() - start) < timeout:
            if self._bl_stop:
                return False
            try:
                item = self._bl_rx_queue.get(timeout=0.0002)
                if item["pf"] == PF_TP_CM and item["d0"] == TP_CM_CTS:
                    return True
            except queue.Empty:
                continue
        return False

    def _bl_wait_endack(self, timeout: float = 5.0) -> bool:
        """队列轮询等待EndAck"""
        start = time.time()
        while (time.time() - start) < timeout:
            if self._bl_stop:
                return False
            try:
                item = self._bl_rx_queue.get(timeout=0.0002)
                if item["pf"] == PF_TP_CM and item["d0"] == TP_CM_END_ACK:
                    return True
            except queue.Empty:
                continue
        return False

    # ---- 固件文件 ----

    def _bl_browse_fw(self):
        path = filedialog.askopenfilename(
            title="选择固件文件",
            filetypes=[("二进制文件", "*.bin"), ("所有文件", "*.*")]
        )
        if path:
            self.bl_fw_path.delete(0, tk.END)
            self.bl_fw_path.insert(0, path)
            self._bl_load_fw(path)

    def _bl_load_fw(self, path: str):
        try:
            with open(path, "rb") as f:
                self._bl_fw_data = f.read()
            size = len(self._bl_fw_data)
            crc = checksum16(self._bl_fw_data)
            self.bl_fw_info.config(text=f"大小: {size} 字节 ({size/1024:.1f} KB)")
            self.bl_checksum_label.config(text=f"Checksum16: 0x{crc:04X}")
            self._bl_log(f"[固件] 加载 {os.path.basename(path)}, {size} 字节, CRC=0x{crc:04X}")
        except OSError as e:
            messagebox.showerror("错误", f"读取固件失败: {e}")
            self._bl_fw_data = b""

    # ---- 协议步骤 ----

    def _bl_app_reset(self):
        """发送APP复位命令 (PF=0xE1, data[0]=0x01), 让APP进入Bootloader"""
        if not self._check_connected():
            return
        self._bl_log("[APP复位] 发送APP复位命令 (PF=0xE1, cmd=0x01)...")
        dev = self._bl_get_dev_addr()
        pc = self._bl_get_pc_addr()
        data = bytes([0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF])
        self._bl_send_can(PF_BOOT_JUMP_APP, dev, pc, data)

    def _bl_boot_jump(self):
        """发送BOOT跳转APP命令 (PF=0xE1, data[0]=0x02), 不等待应答"""
        if not self._check_connected():
            return
        self._bl_log("[BOOT跳转] 发送跳转APP命令 (PF=0xE1, cmd=0x02)...")
        dev = self._bl_get_dev_addr()
        pc = self._bl_get_pc_addr()
        data = bytes([0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF])
        self._bl_send_can(PF_BOOT_JUMP_APP, dev, pc, data)


    def _bl_one_click(self):
        """开始升级: 重启 → 开始 → 传输 → 校验 → 重启"""
        if not self._check_connected():
            return
        if not self._bl_fw_data:
            messagebox.showwarning("警告", "请先加载固件文件")
            return
        if self._bl_running:
            messagebox.showinfo("提示", "升级正在进行中")
            return
        self._bl_stop = False
        self._bl_running = True
        threading.Thread(target=self._bl_one_click_thread, daemon=True).start()

    def _bl_one_click_thread(self):
        try:
            self._bl_run_upgrade()
        finally:
            self._bl_running = False

    def _bl_run_upgrade(self):
        self.root.after(0, lambda: self._bl_log("=" * 40))
        self.root.after(0, lambda: self._bl_log("[升级] 开始..."))

        dev = self._bl_get_dev_addr()
        pc = self._bl_get_pc_addr()

        # Step 0: 下载前发送APP复位命令 (PF=0xE1, cmd=0x01)
        self.root.after(0, lambda: self._bl_log("[0/4] 发送APP复位命令 (PF=0xE1, cmd=0x01)..."))
        self._bl_app_reset()
        time.sleep(0.1)

        # Step 1: 发送升级前导码 (标准CAN帧, ID=0x780+设备地址)
        self.root.after(0, lambda: self._bl_log("[1/4] 发送前导码..."))
        if not self._bl_send_preamble():
            self.root.after(0, lambda: self._bl_log("[升级] 前导码发送失败, 中止"))
            return
        self.root.after(0, lambda: self._bl_log("[1/4] 前导码已发送"))

        # Step 2: 轮询发送BOOT_CMD_START, 等待设备ACK
        self.root.after(0, lambda: self._bl_log("[2/4] 发送编程请求 (轮询等待设备)..."))
        self.root.after(0, lambda: self._bl_set_progress(0, "等待设备响应..."))
        if self._bl_stop: return self._bl_aborted()

        fw_size = len(self._bl_fw_data)
        send_size = fw_size + 4  # 4字节LE, 末尾4字节为校验值, 设备端会减去4
        start_data = bytes([BOOT_CMD_START,
                            send_size & 0xFF, (send_size >> 8) & 0xFF,
                            (send_size >> 16) & 0xFF, (send_size >> 24) & 0xFF,
                            0xFF, 0xFF, 0xFF])

        deadline = time.time() + 10.0  # 10秒窗口
        while time.time() < deadline:
            if self._bl_stop:
                return self._bl_aborted()
            self._bl_clear_events()
            self._bl_send_can(PF_BOOT_CMD, dev, pc, start_data)
            result = self._bl_wait_ack(0.5)
            if result and result[0] == ACK_POSITIVE:
                break
        else:
            self.root.after(0, lambda: self._bl_log("[升级] 设备无应答, 中止"))
            return
        self.root.after(0, lambda: self._bl_log("[2/4] 设备已准备"))
        self.root.after(0, lambda: self._bl_set_progress(1, "设备已准备, 开始传输..."))

        # Step 3: 传输
        if self._bl_stop: return self._bl_aborted()
        self.root.after(0, lambda: self._bl_log("[3/4] 传输固件..."))
        if not self._bl_transfer_inner():
            return

        # Step 4: 校验
        if self._bl_stop: return self._bl_aborted()
        self.root.after(0, lambda: self._bl_log("[4/4] 校验..."))
        time.sleep(0.05)
        fw_crc = checksum16(self._bl_fw_data)
        data = bytes([BOOT_CMD_VERIFY, fw_crc & 0xFF, (fw_crc >> 8) & 0xFF,
                      0xFF, 0xFF, 0xFF, 0xFF, 0xFF])
        self._bl_clear_events()
        self._bl_send_can(PF_BOOT_CMD, dev, pc, data)
        result = self._bl_wait_ack(5.0)
        if result and result[0] == ACK_POSITIVE:
            self.root.after(0, lambda: self._bl_log("[升级] 升级成功!"))
            self.root.after(0, lambda: self._bl_set_progress(100, "升级完成"))
            # 下载完成后发送BOOT跳转APP命令 (PF=0xE1, cmd=0x02)
            self.root.after(0, lambda: self._bl_log("[升级] 发送BOOT跳转APP命令..."))
            self._bl_boot_jump()
        else:
            self.root.after(0, lambda: self._bl_log("[升级] 校验失败"))
            self.root.after(0, lambda: self._bl_set_progress(95, "校验失败"))

        self.root.after(0, lambda: self._bl_log("=" * 40))

    def _bl_aborted(self):
        self.root.after(0, lambda: self._bl_log("[升级] 已停止"))
        self.root.after(0, lambda: self._bl_set_progress(0, "已停止"))
        self.root.after(0, lambda: self._bl_log("=" * 40))

    def _bl_send_can_raw(self, pf: int, ps: int, sa: int, data: bytes, priority: int = 2):
        """直接写串口, 不经过 _send_raw, 避免线程问题"""
        if not self.ser or not self.ser.is_open:
            return False
        can_id = ((priority & 0x07) << 26) | (1 << 24) | ((pf & 0xFF) << 16) | ((ps & 0xFF) << 8) | (sa & 0xFF)
        frame = bytes([(can_id >> 24) & 0xFF, (can_id >> 16) & 0xFF,
                       (can_id >> 8) & 0xFF, can_id & 0xFF])
        frame += data[:8].ljust(8, b'\x00')
        try:
            self.ser.write(frame)
            return True
        except (serial.SerialException, OSError):
            return False

    def _bl_transfer_inner(self) -> bool:
        """GE协议传输, 每块1KB逐包等CTS, 末包等EndAck, 进度随包更新"""
        try:
            fw = self._bl_fw_data
            fw_size = len(fw)
            dev = self._bl_get_dev_addr()
            pc = self._bl_get_pc_addr()

            BLOCK_SIZE = 1024
            PACKET_DATA_LEN = 7

            total_blocks = (fw_size + BLOCK_SIZE - 1) // BLOCK_SIZE
            total_packets = (fw_size + PACKET_DATA_LEN - 1) // PACKET_DATA_LEN

            self.root.after(0, lambda: self._bl_log(f"[传输] {fw_size} 字节, {total_blocks} 块, {total_packets} 包"))

            t_start = time.time()
            offset = 0
            sent = 0

            for block_idx in range(total_blocks):
                if self._bl_stop:
                    return False

                block_bytes = min(BLOCK_SIZE, fw_size - offset)
                num_p = (block_bytes + PACKET_DATA_LEN - 1) // PACKET_DATA_LEN

                rts_data = struct.pack("<BHB", 16, block_bytes, num_p)
                self._bl_clear_events()
                self._bl_send_can_raw(PF_TP_CM, dev, pc, rts_data, PRIORITY_TP)
                if not self._bl_wait_cts(1.0):
                    self.root.after(0, lambda: self._bl_log("[传输] CTS超时, 中止"))
                    return False

                for p_idx in range(1, num_p + 1):
                    if self._bl_stop:
                        return False
                    p_start = (p_idx - 1) * PACKET_DATA_LEN
                    chunk = fw[offset + p_start : offset + p_start + PACKET_DATA_LEN]

                    self._bl_clear_events()
                    self._bl_send_can_raw(PF_TP_DT, dev, pc, bytes([p_idx]) + chunk, PRIORITY_TP)

                    if p_idx < num_p:
                        ok = self._bl_wait_cts(0.5)
                    else:
                        ok = self._bl_wait_endack(5.0)

                    if not ok:
                        tag = "CTS" if p_idx < num_p else "EndAck"
                        self.root.after(0, lambda: self._bl_log(f"[传输] {tag}超时(seq={p_idx})"))
                        return False

                    sent += 1
                    # 每包更新目标进度, 动画层负责平滑过渡
                    progress = 1 + (sent / total_packets) * 98
                    elapsed = time.time() - t_start
                    self.root.after(0, lambda p=progress, s=sent, t=total_packets, e=elapsed:
                                    self._bl_set_progress(p, f"包 {s}/{t} ({e:.1f}s)"))

                offset += block_bytes

            if self._bl_stop:
                return False

            elapsed = time.time() - t_start
            self.root.after(0, lambda e=elapsed: self._bl_log(f"[传输] 完成, 耗时{e:.1f}s"))
            self.root.after(0, lambda: self._bl_set_progress(99, "传输完成"))
            return True

        except Exception as e:
            self.root.after(0, lambda err=str(e): self._bl_log(f"[传输] 异常: {err}"))
            return False

    def _bl_update_boot_b(self):
        """升级Boot B (PF=BOOT_UPDATA_B)"""
        if not self._check_connected():
            return
        if not self._bl_fw_data:
            messagebox.showwarning("警告", "请先加载固件文件")
            return
        self._bl_log("[Boot B] 发送Boot B升级请求...")
        dev = self._bl_get_dev_addr()
        pc = self._bl_get_pc_addr()
        fw_size = len(self._bl_fw_data)
        send_size = fw_size + 4
        data = bytes([BOOT_CMD_START,
                      send_size & 0xFF, (send_size >> 8) & 0xFF,
                      (send_size >> 16) & 0xFF, (send_size >> 24) & 0xFF,
                      0xFF, 0xFF, 0xFF])
        self._bl_clear_events()
        self._bl_send_can(PF_BOOT_UPDATA_B, dev, pc, data)

        result = self._bl_wait_ack(2.0)
        if result and result[0] == ACK_POSITIVE:
            self._bl_log("[Boot B] 设备确认, 开始传输...")
            threading.Thread(target=self._bl_transfer_inner, daemon=True).start()
        else:
            self._bl_log("[Boot B] 设备拒绝或超时")

    def _bl_jump_boot_b(self):
        """跳转到Boot B"""
        if not self._check_connected():
            return
        self._bl_log("[跳转] 发送跳转Boot B命令...")
        dev = self._bl_get_dev_addr()
        pc = self._bl_get_pc_addr()
        data = bytes([0xFF] * 8)
        self._bl_send_can(PF_BOOT_JUMP_B, dev, pc, data)

    def _check_connected(self) -> bool:
        if not self.ser or not self.ser.is_open:
            messagebox.showwarning("警告", "请先连接串口")
            return False
        return True


# ===================== 入口 =====================
if __name__ == "__main__":
    root = tk.Tk()
    app = USB_Tool_Tester(root)
    root.mainloop()
