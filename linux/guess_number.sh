#!/bin/bash
# ============================================
# 创新猜数字小游戏 - 多模式版
# 功能：多种难度、历史记录、统计功能、提示系统
# 作者：Linux大作业
# ============================================

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# 游戏数据目录
DATA_DIR="$HOME/.guess_number_game"
HISTORY_FILE="$DATA_DIR/history.log"
STATS_FILE="$DATA_DIR/stats.txt"

# 初始化游戏
init_game() {
    mkdir -p "$DATA_DIR"
    touch "$HISTORY_FILE" 2>/dev/null
    touch "$STATS_FILE" 2>/dev/null
}

# 显示游戏标题
show_title() {
    clear
    echo -e "${CYAN}"
    echo "╔═══════════════════════════════════════════╗"
    echo "║                                           ║"
    echo "║      🎯  猜 数 字 大 挑 战  🎯            ║"
    echo "║                                           ║"
    echo "║    Guess The Number Challenge v2.0       ║"
    echo "║                                           ║"
    echo "╚═══════════════════════════════════════════╝"
    echo -e "${NC}"
}

# 显示主菜单
show_menu() {
    echo -e "\n${WHITE}════════════ 主 菜 单 ════════════${NC}"
    echo -e "  ${GREEN}1.${NC} 🟢 简单模式 (1-50)"
    echo -e "  ${YELLOW}2.${NC} 🟡 中等模式 (1-100)"
    echo -e "  ${RED}3.${NC} 🔴 困难模式 (1-500)"
    echo -e "  ${PURPLE}4.${NC} 🟣 极限模式 (1-1000, 限制10次)"
    echo -e "  ${CYAN}5.${NC} 📊 查看统计"
    echo -e "  ${BLUE}6.${NC} 📜 历史记录"
    echo -e "  ${WHITE}7.${NC} ❓ 游戏帮助"
    echo -e "  ${RED}0.${NC} 🚪 退出游戏"
    echo -e "${WHITE}══════════════════════════════════${NC}"
}

# 显示帮助信息
show_help() {
    clear
    show_title
    echo -e "\n${WHITE}════════════ 游戏帮助 ════════════${NC}"
    echo -e "${GREEN}游戏规则：${NC}"
    echo "  1. 系统会随机生成一个数字"
    echo "  2. 你需要猜测这个数字是多少"
    echo "  3. 每次猜测后会提示'大了'或'小了'"
    echo "  4. 猜对即可获胜"
    echo ""
    echo -e "${YELLOW}难度说明：${NC}"
    echo "  🟢 简单: 范围1-50, 无限次猜测"
    echo "  🟡 中等: 范围1-100, 无限次猜测"
    echo "  🔴 困难: 范围1-500, 无限次猜测"
    echo "  🟣 极限: 范围1-1000, 仅10次机会!"
    echo ""
    echo -e "${CYAN}创新功能：${NC}"
    echo "  ⭐ 提示系统: 输入'h'获取范围提示(消耗步数)"
    echo "  ⭐ 智能反馈: 显示猜测趋势图"
    echo "  ⭐ 成就系统: 完成挑战获得成就"
    echo "  ⭐ 统计追踪: 记录你的游戏表现"
    echo ""
    echo -e "${WHITE}══════════════════════════════════${NC}"
    read -p "按回车键返回主菜单..."
}

# 显示统计信息
show_stats() {
    clear
    show_title
    echo -e "\n${WHITE}════════════ 游戏统计 ════════════${NC}"

    if [[ -f "$STATS_FILE" ]]; then
        local total_games=0
        local total_wins=0
        local total_guesses=0
        local best_score=999999
        local mode_stats=""

        while IFS='|' read -r mode result guesses timestamp; do
            ((total_games++))
            if [[ "$result" == "WIN" ]]; then
                ((total_wins++))
                ((total_guesses+=guesses))
                if (( guesses < best_score )); then
                    best_score=$guesses
                fi
            fi
        done < "$STATS_FILE"

        if (( total_games > 0 )); then
            local win_rate=$((total_wins * 100 / total_games))
            local avg_guesses=0
            if (( total_wins > 0 )); then
                avg_guesses=$((total_guesses / total_wins))
            fi

            echo -e "  ${GREEN}总游戏次数:${NC} $total_games"
            echo -e "  ${GREEN}获胜次数:${NC}   $total_wins"
            echo -e "  ${GREEN}胜率:${NC}       ${win_rate}%"
            echo -e "  ${GREEN}平均猜测:${NC}   ${avg_guesses}次"
            if (( best_score < 999999 )); then
                echo -e "  ${YELLOW}最佳记录:${NC}   ${best_score}次"
            fi
        else
            echo -e "  ${YELLOW}暂无游戏记录${NC}"
        fi
    else
        echo -e "  ${YELLOW}暂无游戏记录${NC}"
    fi

    echo -e "${WHITE}══════════════════════════════════${NC}"
    read -p "按回车键返回主菜单..."
}

# 显示历史记录
show_history() {
    clear
    show_title
    echo -e "\n${WHITE}════════════ 历史记录 ════════════${NC}"

    if [[ -f "$HISTORY_FILE" ]] && [[ -s "$HISTORY_FILE" ]]; then
        echo -e "${CYAN}最近10条游戏记录:${NC}"
        echo ""
        echo -e "  模式      结果    猜测次数  时间"
        echo -e "  ─────────────────────────────────"

        tail -10 "$HISTORY_FILE" | while IFS='|' read -r mode result guesses timestamp; do
            local mode_name=""
            local result_color=""
            case $mode in
                "easy") mode_name="简单  " ;;
                "medium") mode_name="中等  " ;;
                "hard") mode_name="困难  " ;;
                "extreme") mode_name="极限  " ;;
            esac

            if [[ "$result" == "WIN" ]]; then
                result_color="${GREEN}胜利${NC}"
            else
                result_color="${RED}失败${NC}"
            fi

            printf "  %-8s %b%-4s%b   %-6s    %s\n" "$mode_name" "$result_color" "" "$guesses次" "$timestamp"
        done
    else
        echo -e "  ${YELLOW}暂无历史记录${NC}"
    fi

    echo -e "${WHITE}══════════════════════════════════${NC}"
    read -p "按回车键返回主菜单..."
}

# 记录游戏结果
log_game() {
    local mode=$1
    local result=$2
    local guesses=$3
    local timestamp=$(date '+%Y-%m-%d %H:%M')

    echo "${mode}|${result}|${guesses}|${timestamp}" >> "$STATS_FILE"
    echo "${mode}|${result}|${guesses}|${timestamp}" >> "$HISTORY_FILE"
}

# 绘制猜测趋势图
draw_trend() {
    local guesses=("$@")
    local count=${#guesses[@]}

    if (( count < 2 )); then
        return
    fi

    echo -e "\n${CYAN}📈 猜测趋势:${NC}"

    for ((i=1; i<count; i++)); do
        local prev=${guesses[$((i-1))]}
        local curr=${guesses[$i]}

        if (( curr > prev )); then
            echo -ne "${RED}↑${NC} "
        elif (( curr < prev )); then
            echo -ne "${GREEN}↓${NC} "
        else
            echo -ne "${YELLOW}=${NC} "
        fi
    done
    echo ""
}

# 核心游戏逻辑
play_game() {
    local mode=$1
    local max_num=$2
    local max_attempts=${3:-999999}
    local mode_name=$4

    local target=$((RANDOM % max_num + 1))
    local attempts=0
    local guess=0
    local history_guesses=()
    local hint_used=0
    local low_bound=1
    local high_bound=$max_num

    clear
    show_title
    echo -e "\n${WHITE}════════════ $mode_name ════════════${NC}"
    echo -e "  ${GREEN}目标范围:${NC} 1 - $max_num"
    echo -e "  ${GREEN}剩余次数:${NC} $max_attempts"
    echo -e "  ${CYAN}提示: 输入 'h' 获取范围提示 (消耗1次猜测)${NC}"
    echo -e "${WHITE}══════════════════════════════════${NC}"

    while (( attempts < max_attempts )); do
        echo -e "\n${YELLOW}第 $((attempts + 1)) 次猜测${NC}"
        echo -ne "${WHITE}请输入你的猜测 (1-$max_num): ${NC}"
        read -r input

        # 处理提示请求
        if [[ "$input" == "h" || "$input" == "H" ]]; then
            ((attempts++))
            ((hint_used++))
            local range=$((high_bound - low_bound))
            local quarter=$((range / 4))

            if (( RANDOM % 2 == 0 )); then
                high_bound=$((target + quarter))
                if (( high_bound > max_num )); then
                    high_bound=$max_num
                fi
                echo -e "${PURPLE}💡 提示: 数字在 $low_bound 到 $high_bound 之间${NC}"
            else
                low_bound=$((target - quarter))
                if (( low_bound < 1 )); then
                    low_bound=1
                fi
                echo -e "${PURPLE}💡 提示: 数字在 $low_bound 到 $high_bound 之间${NC}"
            fi

            # 更新剩余次数显示
            if (( max_attempts < 999999 )); then
                echo -e "${GREEN}剩余次数: $((max_attempts - attempts))${NC}"
            fi
            continue
        fi

        # 验证输入
        if ! [[ "$input" =~ ^[0-9]+$ ]]; then
            echo -e "${RED}❌ 请输入有效的数字!${NC}"
            continue
        fi

        guess=$input
        ((attempts++))
        history_guesses+=("$guess")

        # 判断结果
        if (( guess == target )); then
            echo -e "\n${GREEN}╔═══════════════════════════════════════════╗${NC}"
            echo -e "${GREEN}║                                           ║${NC}"
            echo -e "${GREEN}║        🎉 恭喜你猜对了! 🎉               ║${NC}"
            echo -e "${GREEN}║                                           ║${NC}"
            echo -e "${GREEN}╚═══════════════════════════════════════════╝${NC}"
            echo -e "${WHITE}答案就是: ${YELLOW}${target}${NC}"
            echo -e "${WHITE}猜测次数: ${YELLOW}${attempts}${NC}"
            echo -e "${WHITE}使用提示: ${YELLOW}${hint_used}次${NC}"

            # 绘制趋势图
            draw_trend "${history_guesses[@]}"

            # 评价
            local rating=""
            if (( attempts <= max_num / 10 )); then
                rating="${GREEN}⭐⭐⭐ 大师级!${NC}"
            elif (( attempts <= max_num / 5 )); then
                rating="${YELLOW}⭐⭐ 优秀!${NC}"
            elif (( attempts <= max_num / 2 )); then
                rating="${CYAN}⭐ 良好!${NC}"
            else
                rating="${WHITE}继续努力!${NC}"
            fi
            echo -e "\n${WHITE}评价: ${rating}${NC}"

            log_game "$mode" "WIN" "$attempts"
            break

        elif (( guess < target )); then
            low_bound=$((guess + 1))
            echo -e "${BLUE}📉 太小了! 再试试更大的数字${NC}"
            echo -e "${CYAN}当前范围: $low_bound - $high_bound${NC}"
        else
            high_bound=$((guess - 1))
            echo -e "${RED}📈 太大了! 再试试更小的数字${NC}"
            echo -e "${CYAN}当前范围: $low_bound - $high_bound${NC}"
        fi

        # 显示剩余次数(有限制模式)
        if (( max_attempts < 999999 )); then
            local remaining=$((max_attempts - attempts))
            echo -e "${YELLOW}⚠️  剩余次数: $remaining${NC}"

            if (( remaining <= 3 && remaining > 0 )); then
                echo -e "${RED}⚠️  警告: 次数即将用尽!${NC}"
            fi
        fi

        # 显示趋势
        if (( ${#history_guesses[@]} > 1 )); then
            draw_trend "${history_guesses[@]}"
        fi
    done

    # 次数用尽
    if (( attempts >= max_attempts && guess != target )); then
        echo -e "\n${RED}╔═══════════════════════════════════════════╗${NC}"
        echo -e "${RED}║                                           ║${NC}"
        echo -e "${RED}║        😢 游戏结束!                       ║${NC}"
        echo -e "${RED}║                                           ║${NC}"
        echo -e "${RED}╚═══════════════════════════════════════════╝${NC}"
        echo -e "${WHITE}正确答案是: ${YELLOW}${target}${NC}"
        echo -e "${WHITE}你用了 ${attempts} 次机会${NC}"

        log_game "$mode" "LOSE" "$attempts"
    fi

    echo -e "\n${WHITE}══════════════════════════════════${NC}"
    read -p "按回车键返回主菜单..."
}

# 主程序
main() {
    init_game

    while true; do
        show_title
        show_menu

        echo -ne "${WHITE}请选择: ${NC}"
        read -r choice

        case $choice in
            1)
                play_game "easy" 50 999999 "🟢 简单模式"
                ;;
            2)
                play_game "medium" 100 999999 "🟡 中等模式"
                ;;
            3)
                play_game "hard" 500 999999 "🔴 困难模式"
                ;;
            4)
                play_game "extreme" 1000 10 "🟣 极限模式"
                ;;
            5)
                show_stats
                ;;
            6)
                show_history
                ;;
            7)
                show_help
                ;;
            0)
                clear
                echo -e "${GREEN}感谢游玩! 再见! 👋${NC}"
                exit 0
                ;;
            *)
                echo -e "${RED}无效选择，请重试!${NC}"
                sleep 1
                ;;
        esac
    done
}

# 运行主程序
main
