#!/bin/bash
# ============================================
# Linux 系统工具箱 - 集成菜单 v2.0
# 整合：计算器、猜数字、系统监控、日志分析、文件清理、用户管理、备份工具
# 作者：Linux 大作业
# ============================================

# 检测是否在 WSL 环境中
if grep -q Microsoft /proc/version 2>/dev/null || grep -q WSL /proc/version 2>/dev/null; then
    # WSL 环境：使用 Windows 挂载路径
    SCRIPT_DIR="/mnt/e/linux大作业"
else
    # 普通 Linux 环境：使用当前脚本所在目录
    SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
fi

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m'

show_banner() {
    clear
    echo -e "${CYAN}"
    echo "╔═══════════════════════════════════════════════════╗"
    echo "║                                                   ║"
    echo "║        🖥️  Linux 系 统 工 具 箱  🖥️              ║"
    echo "║                                                   ║"
    echo "║          Integrated System Toolkit               ║"
    echo "║                                                   ║"
    echo "╠═══════════════════════════════════════════════════╣"
    echo "║                                                   ║"
    echo "║    ┌───────────────────────────────────────┐    ║"
    echo "║    │                                       │    ║"
    echo "║    │      欢迎使用 Linux 系统工具箱         │    ║"
    echo "║    │                                       │    ║"
    echo "║    └───────────────────────────────────────┘    ║"
    echo "║                                                   ║"
    echo "╚═══════════════════════════════════════════════════╝"
    echo -e "${NC}"
}

show_main_menu() {
    echo -e "\n${WHITE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${WHITE}              📋 主 菜 单 📋                ${NC}"
    echo -e "${WHITE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo -e "  ${GREEN}1.${NC} 🔢 计算器工具"
    echo -e "     └─ 四则运算、科学计算、进制转换、单位换算"
    echo ""
    echo -e "  ${YELLOW}2.${NC} 🎯 猜数字游戏"
    echo -e "     └─ 四种难度、提示系统、统计记录"
    echo ""
    echo -e "  ${BLUE}3.${NC} 💻 系统监控"
    echo -e "     └─ CPU、内存、磁盘、负载监控与告警"
    echo ""
    echo -e "  ${PURPLE}4.${NC} � 安全日志分析"
    echo -e "     └─ 登录成功/失败统计、IP TopN"
    echo ""
    echo -e "  ${CYAN}5.${NC} 🧹 清理工具"
    echo -e "     └─ 清理空目录、临时文件、HTTP 检测"
    echo ""
    echo -e "  ${GREEN}6.${NC} 👥 用户管理"
    echo -e "     └─ 批量创建用户"
    echo ""
    echo -e "  ${BLUE}7.${NC} � 备份工具"
    echo -e "     └─ 自动备份目录、清理旧备份"
    echo ""
    echo -e "  ${WHITE}8.${NC} ❓ 帮助信息"
    echo ""
    echo -e "  ${RED}0.${NC} 🚪 退出系统"
    echo ""
    echo -e "${WHITE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
}

launch_calculator() {
    local script="$SCRIPT_DIR/calculator.sh"
    if [[ -f "$script" ]]; then
        chmod +x "$script" 2>/dev/null
        bash "$script"
    else
        echo -e "${RED}❌ 计算器脚本不存在：$script${NC}"
        sleep 2
    fi
}

launch_guess_game() {
    local script="$SCRIPT_DIR/guess_number.sh"
    if [[ -f "$script" ]]; then
        chmod +x "$script" 2>/dev/null
        bash "$script"
    else
        echo -e "${RED}❌ 猜数字游戏脚本不存在：$script${NC}"
        sleep 2
    fi
}

launch_sysmon() {
    local script="$SCRIPT_DIR/sysmon.sh"
    if [[ -f "$script" ]]; then
        chmod +x "$script" 2>/dev/null
        echo -e "${CYAN}启动系统监控...${NC}"
        bash "$script"
    else
        echo -e "${RED}❌ 系统监控脚本不存在：$script${NC}"
        sleep 2
    fi
}

launch_authlog() {
    local script="$SCRIPT_DIR/authlog_report.sh"
    if [[ -f "$script" ]]; then
        chmod +x "$script" 2>/dev/null
        echo -e "${PURPLE}启动安全日志分析...${NC}"
        bash "$script"
    else
        echo -e "${RED}❌ 安全日志分析脚本不存在：$script${NC}"
        sleep 2
    fi
}

launch_clean_tool() {
    local script="$SCRIPT_DIR/clean_http_tool.sh"
    if [[ -f "$script" ]]; then
        chmod +x "$script" 2>/dev/null
        
        clear
        echo -e "${CYAN}"
        echo "╔═══════════════════════════════════════════════════╗"
        echo "║           🧹 清 理 工 具                          ║"
        echo "╚═══════════════════════════════════════════════════╝"
        echo -e "${NC}"
        
        echo -e "${WHITE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
        echo -e "${WHITE}              清 理 工 具 菜 单              ${NC}"
        echo -e "${WHITE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
        echo ""
        echo -e "  ${GREEN}1.${NC} 清理空目录"
        echo -e "  ${GREEN}2.${NC} 清理临时文件"
        echo -e "  ${GREEN}3.${NC} HTTP 服务检测"
        echo -e "  ${RED}0.${NC} 返回主菜单"
        echo ""
        
        read -p "请选择操作 [0-3]: " choice
        
        case $choice in
            1)
                read -p "请输入目标目录路径: " target_dir
                bash "$script" clean_dir "$target_dir"
                ;;
            2)
                read -p "请输入目标目录路径: " target_dir
                bash "$script" clean_tmp "$target_dir"
                ;;
            3)
                read -p "请输入要检测的网址或 IP: " target
                bash "$script" http "$target"
                ;;
            0)
                return
                ;;
            *)
                echo -e "${RED}无效选择${NC}"
                sleep 1
                ;;
        esac
    else
        echo -e "${RED}❌ 清理工具脚本不存在：$script${NC}"
        sleep 2
    fi
}

launch_create_users() {
    local script="$SCRIPT_DIR/script1_create_users.sh"
    if [[ -f "$script" ]]; then
        chmod +x "$script" 2>/dev/null
        echo -e "${GREEN}启动用户创建工具...${NC}"
        bash "$script"
    else
        echo -e "${RED}❌ 用户管理脚本不存在：$script${NC}"
        sleep 2
    fi
}

launch_backup() {
    local script="$SCRIPT_DIR/script2_backup.sh"
    if [[ -f "$script" ]]; then
        chmod +x "$script" 2>/dev/null
        echo -e "${BLUE}启动备份工具...${NC}"
        echo -e "${YELLOW}⚠️  注意：请确保已修改脚本中的源目录和目标目录路径${NC}"
        read -p "按回车键继续..."
        bash "$script"
    else
        echo -e "${RED}❌ 备份工具脚本不存在：$script${NC}"
        sleep 2
    fi
}

show_help() {
    clear
    echo -e "${GREEN}"
    echo "╔═══════════════════════════════════════════════════╗"
    echo "║           ❓ 帮 助 信 息                          ║"
    echo "╚═══════════════════════════════════════════════════╝"
    echo -e "${NC}"
    
    echo -e "${WHITE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo "欢迎使用 Linux 系统工具箱"
    echo "这是一个整合多种实用工具的交互式菜单程序"
    echo -e "${WHITE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo -e "${YELLOW}功能说明：${NC}"
    echo ""
    echo -e "${GREEN}1. 计算器工具${NC}"
    echo "   - 四则运算（加、减、乘、除、乘方、取模）"
    echo "   - 科学计算（三角函数、对数、阶乘、根号）"
    echo "   - 进制转换（二、八、十、十六进制互转）"
    echo "   - 单位换算（长度、重量、温度等）"
    echo ""
    echo -e "${GREEN}2. 猜数字游戏${NC}"
    echo "   - 四种难度模式：简单、中等、困难、极限"
    echo "   - 智能提示系统"
    echo "   - 游戏统计和历史记录"
    echo ""
    echo -e "${GREEN}3. 系统监控${NC}"
    echo "   - 磁盘使用率监控"
    echo "   - 内存使用率监控"
    echo "   - CPU 负载监控"
    echo "   - 阈值告警（支持 Webhook）"
    echo ""
    echo -e "${GREEN}4. 安全日志分析${NC}"
    echo "   - 统计登录成功/失败/无效用户"
    echo "   - 提取 Failed password 来源 IP"
    echo "   - 显示 TopN IP 排行"
    echo ""
    echo -e "${GREEN}5. 清理工具${NC}"
    echo "   - 批量删除空目录"
    echo "   - 清理临时文件（tmp、log、bak 等）"
    echo "   - HTTP 服务状态检测"
    echo ""
    echo -e "${GREEN}6. 用户管理${NC}"
    echo "   - 批量创建用户"
    echo "   - 自定义用户前缀和数量"
    echo ""
    echo -e "${GREEN}7. 备份工具${NC}"
    echo "   - 自动打包压缩指定目录"
    echo "   - 按日期命名备份文件"
    echo "   - 自动清理 30 天前旧备份"
    echo ""
    echo -e "${WHITE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${CYAN}运行说明：${NC}"
    echo "  - 在 Linux 环境下运行：bash system_menu.sh"
    echo "  - 部分功能需要 root 权限"
    echo ""
    echo -e "${WHITE}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    read -p "按回车键返回主菜单..."
}

confirm_exit() {
    clear
    echo -e "${CYAN}"
    echo "╔═══════════════════════════════════════════════════╗"
    echo "║                                                   ║"
    echo "║         感谢使用 Linux 系统工具箱！              ║"
    echo "║                                                   ║"
    echo "║              See You Next Time!                   ║"
    echo "║                                                   ║"
    echo "╚═══════════════════════════════════════════════════╝"
    echo -e "${NC}"
    exit 0
}

main() {
    while true; do
        show_banner
        show_main_menu
        
        read -p "请输入选项 [0-8]: " choice
        
        case $choice in
            1)
                launch_calculator
                ;;
            2)
                launch_guess_game
                ;;
            3)
                launch_sysmon
                ;;
            4)
                launch_authlog
                ;;
            5)
                launch_clean_tool
                ;;
            6)
                launch_create_users
                ;;
            7)
                launch_backup
                ;;
            8)
                show_help
                ;;
            0)
                confirm_exit
                ;;
            *)
                echo -e "${RED}❌ 无效选项，请重新选择${NC}"
                sleep 1
                ;;
        esac
    done
}

main
