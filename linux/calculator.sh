#!/bin/bash
# ============================================
# 高级计算器程序
# 功能：基础运算、科学计算、进制转换、历史记录
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
NC='\033[0m'

# 历史记录文件
HISTORY_FILE="$HOME/.calculator_history"

# 初始化
init() {
    touch "$HISTORY_FILE" 2>/dev/null
}

# 显示计算器界面
show_calculator() {
    clear
    echo -e "${CYAN}"
    echo "╔═══════════════════════════════════════════════╗"
    echo "║                                               ║"
    echo "║         🔢  高 级 计 算 器  🔢                ║"
    echo "║                                               ║"
    echo "║       Advanced Calculator v2.0               ║"
    echo "║                                               ║"
    echo "╠═══════════════════════════════════════════════╣"
    echo "║                                               ║"
    echo "║  ┌─────────────────────────────────────┐     ║"
    echo "║  │                                     │     ║"
    echo "║  │         计 算 区 域                  │     ║"
    echo "║  │                                     │     ║"
    echo "║  └─────────────────────────────────────┘     ║"
    echo "║                                               ║"
    echo "╚═══════════════════════════════════════════════╝"
    echo -e "${NC}"
}

# 显示主菜单
show_menu() {
    echo -e "\n${WHITE}════════════ 功能菜单 ════════════${NC}"
    echo -e "  ${GREEN}1.${NC} ➕ 四则运算 (加减乘除)"
    echo -e "  ${GREEN}2.${NC} 🔬 科学计算 (幂、根、三角函数)"
    echo -e "  ${GREEN}3.${NC} 📊 进制转换"
    echo -e "  ${GREEN}4.${NC} 💾 单位换算"
    echo -e "  ${GREEN}5.${NC} 📜 查看历史记录"
    echo -e "  ${GREEN}6.${NC} 🗑️  清空历史记录"
    echo -e "  ${GREEN}7.${NC} ❓ 使用帮助"
    echo -e "  ${RED}0.${NC} 🚪 退出计算器"
    echo -e "${WHITE}══════════════════════════════════${NC}"
}

# 显示帮助
show_help() {
    clear
    show_calculator
    echo -e "\n${WHITE}════════════ 使用帮助 ════════════${NC}"
    echo -e "${GREEN}四则运算支持:${NC}"
    echo "  + 加法    - 减法    * 乘法    / 除法"
    echo "  % 取模    ** 幂运算"
    echo ""
    echo -e "${GREEN}科学计算支持:${NC}"
    echo "  平方根、立方根、n次方根"
    echo "  sin, cos, tan (角度制)"
    echo "  对数运算 (log, ln)"
    echo "  阶乘 (!)"
    echo ""
    echo -e "${GREEN}进制转换支持:${NC}"
    echo "  二进制、八进制、十进制、十六进制互转"
    echo ""
    echo -e "${GREEN}单位换算支持:${NC}"
    echo "  长度、重量、温度等常用单位"
    echo ""
    echo -e "${WHITE}══════════════════════════════════${NC}"
    read -p "按回车键返回主菜单..."
}

# 保存历史记录
save_history() {
    local expression=$1
    local result=$2
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S')
    echo "[$timestamp] $expression = $result" >> "$HISTORY_FILE"
}

# 查看历史记录
view_history() {
    clear
    show_calculator
    echo -e "\n${WHITE}════════════ 历史记录 ════════════${NC}"

    if [[ -f "$HISTORY_FILE" ]] && [[ -s "$HISTORY_FILE" ]]; then
        echo -e "${CYAN}最近的计算记录:${NC}\n"
        tail -20 "$HISTORY_FILE" | while read -r line; do
            echo -e "  ${YELLOW}$line${NC}"
        done
    else
        echo -e "  ${YELLOW}暂无历史记录${NC}"
    fi

    echo -e "${WHITE}══════════════════════════════════${NC}"
    read -p "按回车键返回主菜单..."
}

# 清空历史记录
clear_history() {
    echo -n "" > "$HISTORY_FILE"
    echo -e "${GREEN}✅ 历史记录已清空!${NC}"
    sleep 1
}

# 四则运算
basic_calc() {
    while true; do
        clear
        show_calculator
        echo -e "\n${WHITE}════════════ 四则运算 ════════════${NC}"
        echo -e "${CYAN}支持: + - * / % ** (括号)${NC}"
        echo -e "${YELLOW}输入 'q' 返回主菜单${NC}"
        echo -e "${WHITE}══════════════════════════════════${NC}"

        echo -ne "\n${GREEN}请输入表达式: ${NC}"
        read -r expression

        # 退出检查
        if [[ "$expression" == "q" || "$expression" == "Q" ]]; then
            break
        fi

        # 空输入检查
        if [[ -z "$expression" ]]; then
            continue
        fi

        # 计算并显示结果
        echo -e "\n${WHITE}计算中...${NC}"

        # 使用 bc 进行计算
        result=$(echo "scale=10; $expression" 2>/dev/null | bc -l 2>/dev/null)

        if [[ $? -eq 0 && -n "$result" ]]; then
            # 去除尾部多余的0
            result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')

            echo -e "${GREEN}╔══════════════════════════════════════${NC}"
            echo -e "${GREEN}║                                      ║${NC}"
            printf "${GREEN}║  %-36s║${NC}\n" "$expression = $result"
            echo -e "${GREEN}║                                      ║${NC}"
            echo -e "${GREEN}╚══════════════════════════════════════${NC}"

            save_history "$expression" "$result"
        else
            echo -e "${RED}❌ 表达式错误，请检查输入!${NC}"
        fi

        echo ""
        read -p "按回车键继续..."
    done
}

# 科学计算
scientific_calc() {
    while true; do
        clear
        show_calculator
        echo -e "\n${WHITE}════════════ 科学计算 ════════════${NC}"
        echo -e "  ${GREEN}1.${NC} 平方根 (√)"
        echo -e "  ${GREEN}2.${NC} 立方根 (∛)"
        echo -e "  ${GREEN}3.${NC} n次方根"
        echo -e "  ${GREEN}4.${NC} 幂运算 (x^y)"
        echo -e "  ${GREEN}5.${NC} 正弦 (sin)"
        echo -e "  ${GREEN}6.${NC} 余弦 (cos)"
        echo -e "  ${GREEN}7.${NC} 正切 (tan)"
        echo -e "  ${GREEN}8.${NC} 对数 (log)"
        echo -e "  ${GREEN}9.${NC} 自然对数 (ln)"
        echo -e "  ${GREEN}10.${NC} 阶乘 (!)"
        echo -e "  ${GREEN}11.${NC} 绝对值 (abs)"
        echo -e "  ${YELLOW}0.${NC} 返回主菜单"
        echo -e "${WHITE}══════════════════════════════════${NC}"

        echo -ne "${GREEN}请选择: ${NC}"
        read -r choice

        case $choice in
            1)
                echo -ne "${CYAN}请输入数字: ${NC}"
                read -r num
                if [[ "$num" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    if (( $(echo "$num >= 0" | bc -l) )); then
                        result=$(echo "scale=10; sqrt($num)" | bc -l)
                        result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')
                        echo -e "${GREEN}√$num = $result${NC}"
                        save_history "sqrt($num)" "$result"
                    else
                        echo -e "${RED}❌ 负数没有实数平方根!${NC}"
                    fi
                else
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                fi
                ;;
            2)
                echo -ne "${CYAN}请输入数字: ${NC}"
                read -r num
                if [[ "$num" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    result=$(echo "scale=10; $num ^ (1/3)" | bc -l)
                    result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')
                    echo -e "${GREEN}∛$num = $result${NC}"
                    save_history "cbrt($num)" "$result"
                else
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                fi
                ;;
            3)
                echo -ne "${CYAN}请输入数字: ${NC}"
                read -r num
                echo -ne "${CYAN}请输入根指数: ${NC}"
                read -r n
                if [[ "$num" =~ ^-?[0-9]+\.?[0-9]*$ ]] && [[ "$n" =~ ^[0-9]+$ ]]; then
                    result=$(echo "scale=10; $num ^ (1/$n)" | bc -l)
                    result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')
                    echo -e "${GREEN}${num}^(1/$n) = $result${NC}"
                    save_history "${num}^(1/$n)" "$result"
                else
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                fi
                ;;
            4)
                echo -ne "${CYAN}请输入底数: ${NC}"
                read -r base
                echo -ne "${CYAN}请输入指数: ${NC}"
                read -r exp
                if [[ "$base" =~ ^-?[0-9]+\.?[0-9]*$ ]] && [[ "$exp" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    result=$(echo "scale=10; $base ^ $exp" | bc -l)
                    result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')
                    echo -e "${GREEN}${base}^${exp} = $result${NC}"
                    save_history "${base}^${exp}" "$result"
                else
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                fi
                ;;
            5)
                echo -ne "${CYAN}请输入角度: ${NC}"
                read -r angle
                if [[ "$angle" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    # 角度转弧度
                    radians=$(echo "scale=10; $angle * 3.14159265358979323846 / 180" | bc -l)
                    result=$(echo "scale=10; s($radians)" | bc -l)
                    result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')
                    echo -e "${GREEN}sin($angle°) = $result${NC}"
                    save_history "sin($angle°)" "$result"
                else
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                fi
                ;;
            6)
                echo -ne "${CYAN}请输入角度: ${NC}"
                read -r angle
                if [[ "$angle" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    radians=$(echo "scale=10; $angle * 3.14159265358979323846 / 180" | bc -l)
                    result=$(echo "scale=10; c($radians)" | bc -l)
                    result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')
                    echo -e "${GREEN}cos($angle°) = $result${NC}"
                    save_history "cos($angle°)" "$result"
                else
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                fi
                ;;
            7)
                echo -ne "${CYAN}请输入角度: ${NC}"
                read -r angle
                if [[ "$angle" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    radians=$(echo "scale=10; $angle * 3.14159265358979323846 / 180" | bc -l)
                    result=$(echo "scale=10; s($radians) / c($radians)" | bc -l)
                    result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')
                    echo -e "${GREEN}tan($angle°) = $result${NC}"
                    save_history "tan($angle°)" "$result"
                else
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                fi
                ;;
            8)
                echo -ne "${CYAN}请输入数字: ${NC}"
                read -r num
                echo -ne "${CYAN}请输入底数(默认10): ${NC}"
                read -r base
                base=${base:-10}
                if [[ "$num" =~ ^[0-9]+\.?[0-9]*$ ]] && (( $(echo "$num > 0" | bc -l) )); then
                    result=$(echo "scale=10; l($num) / l($base)" | bc -l)
                    result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')
                    echo -e "${GREEN}log_$base($num) = $result${NC}"
                    save_history "log_$base($num)" "$result"
                else
                    echo -e "${RED}❌ 请输入有效的正数!${NC}"
                fi
                ;;
            9)
                echo -ne "${CYAN}请输入数字: ${NC}"
                read -r num
                if [[ "$num" =~ ^[0-9]+\.?[0-9]*$ ]] && (( $(echo "$num > 0" | bc -l) )); then
                    result=$(echo "scale=10; l($num)" | bc -l)
                    result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')
                    echo -e "${GREEN}ln($num) = $result${NC}"
                    save_history "ln($num)" "$result"
                else
                    echo -e "${RED}❌ 请输入有效的正数!${NC}"
                fi
                ;;
            10)
                echo -ne "${CYAN}请输入非负整数: ${NC}"
                read -r num
                if [[ "$num" =~ ^[0-9]+$ ]]; then
                    if (( num <= 170 )); then
                        result=1
                        for ((i=2; i<=num; i++)); do
                            result=$((result * i))
                        done
                        echo -e "${GREEN}$num! = $result${NC}"
                        save_history "$num!" "$result"
                    else
                        echo -e "${RED}❌ 数字太大(最大支持170)!${NC}"
                    fi
                else
                    echo -e "${RED}❌ 请输入非负整数!${NC}"
                fi
                ;;
            11)
                echo -ne "${CYAN}请输入数字: ${NC}"
                read -r num
                if [[ "$num" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    result=$(echo "scale=10; if ($num < 0) -$num else $num" | bc -l)
                    result=$(echo "$result" | sed 's/\.0*$//; s/\.\([0-9]*[1-9]\)0*$/.\1/')
                    echo -e "${GREEN}|$num| = $result${NC}"
                    save_history "abs($num)" "$result"
                else
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                fi
                ;;
            0)
                break
                ;;
            *)
                echo -e "${RED}无效选择!${NC}"
                ;;
        esac

        echo ""
        read -p "按回车键继续..."
    done
}

# 进制转换
base_conversion() {
    while true; do
        clear
        show_calculator
        echo -e "\n${WHITE}════════════ 进制转换 ════════════${NC}"
        echo -e "  ${GREEN}1.${NC} 十进制 → 二进制"
        echo -e "  ${GREEN}2.${NC} 十进制 → 八进制"
        echo -e "  ${GREEN}3.${NC} 十进制 → 十六进制"
        echo -e "  ${GREEN}4.${NC} 二进制 → 十进制"
        echo -e "  ${GREEN}5.${NC} 八进制 → 十进制"
        echo -e "  ${GREEN}6.${NC} 十六进制 → 十进制"
        echo -e "  ${GREEN}7.${NC} 自定义转换"
        echo -e "  ${YELLOW}0.${NC} 返回主菜单"
        echo -e "${WHITE}══════════════════════════════════${NC}"

        echo -ne "${GREEN}请选择: ${NC}"
        read -r choice

        case $choice in
            1)
                echo -ne "${CYAN}请输入十进制数: ${NC}"
                read -r num
                if [[ "$num" =~ ^[0-9]+$ ]]; then
                    result=$(echo "obase=2; $num" | bc)
                    echo -e "${GREEN}$num (十进制) = $result (二进制)${NC}"
                    save_history "$num -> binary" "$result"
                else
                    echo -e "${RED}❌ 请输入有效的十进制整数!${NC}"
                fi
                ;;
            2)
                echo -ne "${CYAN}请输入十进制数: ${NC}"
                read -r num
                if [[ "$num" =~ ^[0-9]+$ ]]; then
                    result=$(echo "obase=8; $num" | bc)
                    echo -e "${GREEN}$num (十进制) = $result (八进制)${NC}"
                    save_history "$num -> octal" "$result"
                else
                    echo -e "${RED}❌ 请输入有效的十进制整数!${NC}"
                fi
                ;;
            3)
                echo -ne "${CYAN}请输入十进制数: ${NC}"
                read -r num
                if [[ "$num" =~ ^[0-9]+$ ]]; then
                    result=$(echo "obase=16; $num" | bc)
                    echo -e "${GREEN}$num (十进制) = $result (十六进制)${NC}"
                    save_history "$num -> hex" "$result"
                else
                    echo -e "${RED}❌ 请输入有效的十进制整数!${NC}"
                fi
                ;;
            4)
                echo -ne "${CYAN}请输入二进制数: ${NC}"
                read -r num
                if [[ "$num" =~ ^[01]+$ ]]; then
                    result=$(echo "ibase=2; $num" | bc)
                    echo -e "${GREEN}$num (二进制) = $result (十进制)${NC}"
                    save_history "$num (binary) -> decimal" "$result"
                else
                    echo -e "${RED}❌ 请输入有效的二进制数(只含0和1)!${NC}"
                fi
                ;;
            5)
                echo -ne "${CYAN}请输入八进制数: ${NC}"
                read -r num
                if [[ "$num" =~ ^[0-7]+$ ]]; then
                    result=$(echo "ibase=8; $num" | bc)
                    echo -e "${GREEN}$num (八进制) = $result (十进制)${NC}"
                    save_history "$num (octal) -> decimal" "$result"
                else
                    echo -e "${RED}❌ 请输入有效的八进制数(只含0-7)!${NC}"
                fi
                ;;
            6)
                echo -ne "${CYAN}请输入十六进制数: ${NC}"
                read -r num
                if [[ "$num" =~ ^[0-9A-Fa-f]+$ ]]; then
                    num_upper=$(echo "$num" | tr '[:lower:]' '[:upper:]')
                    result=$(echo "ibase=16; $num_upper" | bc)
                    echo -e "${GREEN}$num (十六进制) = $result (十进制)${NC}"
                    save_history "$num (hex) -> decimal" "$result"
                else
                    echo -e "${RED}❌ 请输入有效的十六进制数(0-9, A-F)!${NC}"
                fi
                ;;
            7)
                echo -ne "${CYAN}请输入源进制(2-16): ${NC}"
                read -r from_base
                echo -ne "${CYAN}请输入目标进制(2-16): ${NC}"
                read -r to_base
                echo -ne "${CYAN}请输入数字: ${NC}"
                read -r num

                if [[ "$from_base" =~ ^[2-9]$|^1[0-6]$ ]] && [[ "$to_base" =~ ^[2-9]$|^1[0-6]$ ]]; then
                    # 先转为十进制
                    if (( from_base == 10 )); then
                        decimal=$num
                    else
                        num_upper=$(echo "$num" | tr '[:lower:]' '[:upper:]')
                        decimal=$(echo "ibase=$from_base; $num_upper" | bc 2>/dev/null)
                    fi

                    if [[ -n "$decimal" ]]; then
                        # 再转为目标进制
                        if (( to_base == 10 )); then
                            result=$decimal
                        else
                            result=$(echo "obase=$to_base; $decimal" | bc)
                        fi
                        echo -e "${GREEN}$num ($from_base进制) = $result ($to_base进制)${NC}"
                        save_history "$num (base$from_base) -> base$to_base" "$result"
                    else
                        echo -e "${RED}❌ 转换失败，请检查输入!${NC}"
                    fi
                else
                    echo -e "${RED}❌ 进制必须在2-16之间!${NC}"
                fi
                ;;
            0)
                break
                ;;
            *)
                echo -e "${RED}无效选择!${NC}"
                ;;
        esac

        echo ""
        read -p "按回车键继续..."
    done
}

# 单位换算
unit_conversion() {
    while true; do
        clear
        show_calculator
        echo -e "\n${WHITE}════════════ 单位换算 ════════════${NC}"
        echo -e "  ${GREEN}1.${NC} 长度换算"
        echo -e "  ${GREEN}2.${NC} 重量换算"
        echo -e "  ${GREEN}3.${NC} 温度换算"
        echo -e "  ${GREEN}4.${NC} 面积换算"
        echo -e "  ${GREEN}5.${NC} 体积换算"
        echo -e "  ${YELLOW}0.${NC} 返回主菜单"
        echo -e "${WHITE}══════════════════════════════════${NC}"

        echo -ne "${GREEN}请选择: ${NC}"
        read -r choice

        case $choice in
            1) # 长度换算
                echo -e "\n${CYAN}长度换算:${NC}"
                echo "  1. 米(m) → 厘米(cm)"
                echo "  2. 米(m) → 毫米(mm)"
                echo "  3. 米(m) → 千米(km)"
                echo "  4. 米(m) → 英寸(in)"
                echo "  5. 米(m) → 英尺(ft)"
                echo "  6. 千米(km) → 英里(mi)"
                echo "  7. 英寸(in) → 厘米(cm)"
                echo -ne "${CYAN}请选择换算类型: ${NC}"
                read -r sub

                echo -ne "${CYAN}请输入数值: ${NC}"
                read -r num

                if [[ ! "$num" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                    read -p "按回车键继续..."
                    continue
                fi

                case $sub in
                    1) result=$(echo "scale=4; $num * 100" | bc); echo -e "${GREEN}$num m = $result cm${NC}" ;;
                    2) result=$(echo "scale=4; $num * 1000" | bc); echo -e "${GREEN}$num m = $result mm${NC}" ;;
                    3) result=$(echo "scale=6; $num / 1000" | bc); echo -e "${GREEN}$num m = $result km${NC}" ;;
                    4) result=$(echo "scale=4; $num * 39.3701" | bc); echo -e "${GREEN}$num m = $result in${NC}" ;;
                    5) result=$(echo "scale=4; $num * 3.28084" | bc); echo -e "${GREEN}$num m = $result ft${NC}" ;;
                    6) result=$(echo "scale=4; $num * 0.621371" | bc); echo -e "${GREEN}$num km = $result mi${NC}" ;;
                    7) result=$(echo "scale=4; $num * 2.54" | bc); echo -e "${GREEN}$num in = $result cm${NC}" ;;
                    *) echo -e "${RED}无效选择!${NC}" ;;
                esac
                ;;
            2) # 重量换算
                echo -e "\n${CYAN}重量换算:${NC}"
                echo "  1. 千克(kg) → 克(g)"
                echo "  2. 千克(kg) → 毫克(mg)"
                echo "  3. 千克(kg) → 吨(t)"
                echo "  4. 千克(kg) → 磅(lb)"
                echo "  5. 千克(kg) → 盎司(oz)"
                echo -ne "${CYAN}请选择换算类型: ${NC}"
                read -r sub

                echo -ne "${CYAN}请输入数值: ${NC}"
                read -r num

                if [[ ! "$num" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                    read -p "按回车键继续..."
                    continue
                fi

                case $sub in
                    1) result=$(echo "scale=4; $num * 1000" | bc); echo -e "${GREEN}$num kg = $result g${NC}" ;;
                    2) result=$(echo "scale=4; $num * 1000000" | bc); echo -e "${GREEN}$num kg = $result mg${NC}" ;;
                    3) result=$(echo "scale=6; $num / 1000" | bc); echo -e "${GREEN}$num kg = $result t${NC}" ;;
                    4) result=$(echo "scale=4; $num * 2.20462" | bc); echo -e "${GREEN}$num kg = $result lb${NC}" ;;
                    5) result=$(echo "scale=4; $num * 35.274" | bc); echo -e "${GREEN}$num kg = $result oz${NC}" ;;
                    *) echo -e "${RED}无效选择!${NC}" ;;
                esac
                ;;
            3) # 温度换算
                echo -e "\n${CYAN}温度换算:${NC}"
                echo "  1. 摄氏(°C) → 华氏(°F)"
                echo "  2. 摄氏(°C) → 开尔文(K)"
                echo "  3. 华氏(°F) → 摄氏(°C)"
                echo "  4. 华氏(°F) → 开尔文(K)"
                echo "  5. 开尔文(K) → 摄氏(°C)"
                echo "  6. 开尔文(K) → 华氏(°F)"
                echo -ne "${CYAN}请选择换算类型: ${NC}"
                read -r sub

                echo -ne "${CYAN}请输入数值: ${NC}"
                read -r num

                if [[ ! "$num" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                    read -p "按回车键继续..."
                    continue
                fi

                case $sub in
                    1) result=$(echo "scale=2; $num * 9 / 5 + 32" | bc); echo -e "${GREEN}$num °C = $result °F${NC}" ;;
                    2) result=$(echo "scale=2; $num + 273.15" | bc); echo -e "${GREEN}$num °C = $result K${NC}" ;;
                    3) result=$(echo "scale=2; ($num - 32) * 5 / 9" | bc); echo -e "${GREEN}$num °F = $result °C${NC}" ;;
                    4) result=$(echo "scale=2; ($num - 32) * 5 / 9 + 273.15" | bc); echo -e "${GREEN}$num °F = $result K${NC}" ;;
                    5) result=$(echo "scale=2; $num - 273.15" | bc); echo -e "${GREEN}$num K = $result °C${NC}" ;;
                    6) result=$(echo "scale=2; ($num - 273.15) * 9 / 5 + 32" | bc); echo -e "${GREEN}$num K = $result °F${NC}" ;;
                    *) echo -e "${RED}无效选择!${NC}" ;;
                esac
                ;;
            4) # 面积换算
                echo -e "\n${CYAN}面积换算:${NC}"
                echo "  1. 平方米(m²) → 平方厘米(cm²)"
                echo "  2. 平方米(m²) → 平方千米(km²)"
                echo "  3. 平方米(m²) → 公顷(ha)"
                echo "  4. 平方米(m²) → 平方英尺(ft²)"
                echo "  5. 平方米(m²) → 亩"
                echo -ne "${CYAN}请选择换算类型: ${NC}"
                read -r sub

                echo -ne "${CYAN}请输入数值: ${NC}"
                read -r num

                if [[ ! "$num" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                    read -p "按回车键继续..."
                    continue
                fi

                case $sub in
                    1) result=$(echo "scale=4; $num * 10000" | bc); echo -e "${GREEN}$num m² = $result cm²${NC}" ;;
                    2) result=$(echo "scale=8; $num / 1000000" | bc); echo -e "${GREEN}$num m² = $result km²${NC}" ;;
                    3) result=$(echo "scale=6; $num / 10000" | bc); echo -e "${GREEN}$num m² = $result ha${NC}" ;;
                    4) result=$(echo "scale=4; $num * 10.7639" | bc); echo -e "${GREEN}$num m² = $result ft²${NC}" ;;
                    5) result=$(echo "scale=6; $num * 0.0015" | bc); echo -e "${GREEN}$num m² = $result 亩${NC}" ;;
                    *) echo -e "${RED}无效选择!${NC}" ;;
                esac
                ;;
            5) # 体积换算
                echo -e "\n${CYAN}体积换算:${NC}"
                echo "  1. 升(L) → 毫升(mL)"
                echo "  2. 升(L) → 立方米(m³)"
                echo "  3. 升(L) → 加仑(gal)"
                echo "  4. 升(L) → 品脱(pt)"
                echo "  5. 立方米(m³) → 升(L)"
                echo -ne "${CYAN}请选择换算类型: ${NC}"
                read -r sub

                echo -ne "${CYAN}请输入数值: ${NC}"
                read -r num

                if [[ ! "$num" =~ ^-?[0-9]+\.?[0-9]*$ ]]; then
                    echo -e "${RED}❌ 请输入有效数字!${NC}"
                    read -p "按回车键继续..."
                    continue
                fi

                case $sub in
                    1) result=$(echo "scale=4; $num * 1000" | bc); echo -e "${GREEN}$num L = $result mL${NC}" ;;
                    2) result=$(echo "scale=6; $num / 1000" | bc); echo -e "${GREEN}$num L = $result m³${NC}" ;;
                    3) result=$(echo "scale=4; $num * 0.264172" | bc); echo -e "${GREEN}$num L = $result gal${NC}" ;;
                    4) result=$(echo "scale=4; $num * 2.11338" | bc); echo -e "${GREEN}$num L = $result pt${NC}" ;;
                    5) result=$(echo "scale=4; $num * 1000" | bc); echo -e "${GREEN}$num m³ = $result L${NC}" ;;
                    *) echo -e "${RED}无效选择!${NC}" ;;
                esac
                ;;
            0)
                break
                ;;
            *)
                echo -e "${RED}无效选择!${NC}"
                ;;
        esac

        echo ""
        read -p "按回车键继续..."
    done
}

# 主程序
main() {
    init

    while true; do
        show_calculator
        show_menu

        echo -ne "${WHITE}请选择功能: ${NC}"
        read -r choice

        case $choice in
            1)
                basic_calc
                ;;
            2)
                scientific_calc
                ;;
            3)
                base_conversion
                ;;
            4)
                unit_conversion
                ;;
            5)
                view_history
                ;;
            6)
                clear_history
                ;;
            7)
                show_help
                ;;
            0)
                clear
                echo -e "${GREEN}感谢使用高级计算器! 再见! 👋${NC}"
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
