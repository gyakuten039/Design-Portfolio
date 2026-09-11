#!/usr/bin/env bash
set -euo pipefail

# 安全日志报表：日志统计 + TopN IP + 终端输出

SCRIPT_NAME="$(basename "$0")"                                  # 脚本名称
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"   # 项目根目录

DEFAULT_CONFIG="$ROOT_DIR/logreport.conf"                 # 默认配置文件

usage() {
  cat <<EOF
用法: $SCRIPT_NAME [-c 配置文件] [-w] [-l 日志文件] [-n TopN]

说明: 统计登录成功/失败/无效用户, 提取 Failed password 来源 IP TopN, 输出表格。
EOF
}

load_config() {
  local cfg="$1"

  # shellcheck disable=SC1090
  [[ -f "$cfg" ]] && source "$cfg" || true
  : "${TOP_N:=10}"
}

pick_logfile_whiptail() {
  local sample="$ROOT_DIR/data/sample_auth.log"  # 兜底样例
  local default_path="/sample_auth.log"
  [[ -f "$default_path" ]] || default_path="/var/log/syslog"
  [[ -f "$default_path" ]] || default_path="$sample"
  local choice

  choice=$(whiptail --title "日志报表" --menu "请选择日志文件" 15 90 6 \
    "/var/log/auth.log" "系统认证日志 auth.log" \
    "/var/log/syslog" "系统日志 syslog" \
    "$sample" "样例 auth.log" \
    "CUSTOM" "手动输入路径" \
    3>&1 1>&2 2>&3) || exit 1

  if [[ "$choice" == "CUSTOM" ]]; then
    choice=$(whiptail --title "日志报表" --inputbox "请输入日志文件路径" 10 90 "$default_path" 3>&1 1>&2 2>&3) || exit 1
  fi
  echo "$choice"
}

require_root_for_system_logs() {
  local logfile="$1"
  if [[ "$logfile" == /var/log/* && ! -r "$logfile" ]]; then
    echo "读取 $logfile 需要更高权限" >&2
    echo "请使用 sudo 运行" >&2
    exit 1
  fi
  return 0
}

extract_failed_ips() {
  local logfile="$1" out_ips="$2" use_gauge="$3"

  : >"$out_ips"  # 先清空输出文件
  if (( use_gauge == 1 )); then
    local total
    total=$(wc -l <"$logfile" | tr -d ' ')
    [[ "$total" =~ ^[0-9]+$ ]] || total=1
    (( total > 0 )) || total=1

    awk -v out="$out_ips" -v t="$total" '
      /Failed password/ {
        for(i=1;i<=NF;i++) if($i=="from"){ h=$(i+1); gsub(/[;,]$/, "", h); if(h!="") print h >> out; break }
      }
      /authentication failure/ {
        for(i=1;i<=NF;i++) if($i ~ /^rhost=/){ split($i,a,"="); h=a[2]; gsub(/[;,]$/, "", h); if(h!="") print h >> out; break }
      }
      NR%200==0 { p=int(NR*100/t); if(p>100)p=100; print p }
      END{ print 100 }
    ' "$logfile" | whiptail --title "日志报表" --gauge "正在解析日志..." 8 70 0
    return 0
  fi

  # 流式处理
  awk '
    /Failed password/ {
      for(i=1;i<=NF;i++) if($i=="from"){ h=$(i+1); gsub(/[;,]$/, "", h); if(h!="") print h; break }
    }
    /authentication failure/ {
      for(i=1;i<=NF;i++) if($i ~ /^rhost=/){ split($i,a,"="); h=a[2]; gsub(/[;,]$/, "", h); if(h!="") print h; break }
    }
  ' "$logfile" >"$out_ips" || true
}

top_rows_tsv() {
  local ips_file="$1" top_n="$2"
  sort "$ips_file" | uniq -c | sort -nr | head -n "$top_n" | awk '{printf "%s\t%s\n", $2, $1}' || true  # TopN
}

print_report() {
  local logfile="$1" ts="$2" top_n="$3" success="$4" failed="$5" invalid="$6" rows_tsv="$7" cmd="$8"

  cat <<EOF
# 系统安全日志报表

- Log: $logfile
- Time: $ts
- TopN: $top_n

## 1. Overview

| KPI | Count | Notes |
| --- | --- | --- |
| Login Success | $success | Accepted password / Accepted publickey |
| Login Failed | $failed | Failed password |
| Invalid User | $invalid | invalid user |

Command:

~~~bash
$cmd
~~~

## 2. TopN IP

| IP | Failed Count |
| --- | --- |
EOF

  # heredoc + while: 动态生成表格
  while IFS=$'\t' read -r ip cnt; do
    [[ -n "$ip" ]] || continue
    echo "| $ip | $cnt |"
  done <<<"$rows_tsv"
}

main() {
  local cfg="$DEFAULT_CONFIG" use_menu=0
  local logfile="/var/log/auth.log"
  [[ -f "$logfile" ]] || logfile="$ROOT_DIR/data/sample_auth.log"
  local top_n=""

  while getopts ":c:wl:n:h" opt; do  # 参数解析
    case "$opt" in
      c) cfg="$OPTARG" ;;
      w) use_menu=1 ;;
      l) logfile="$OPTARG" ;;
      n) top_n="$OPTARG" ;;
      h) usage; exit 0 ;;
      \?) usage; exit 2 ;;
    esac
  done

  load_config "$cfg"
  [[ -n "$top_n" ]] || top_n="$TOP_N"

  if (( use_menu == 1 )); then  # whiptail 菜单 + gauge
    logfile=$(pick_logfile_whiptail)
  fi

  [[ -f "$logfile" ]] || { echo "日志文件不存在: $logfile" >&2; exit 1; }
  require_root_for_system_logs "$logfile"

  local ts
  ts=$(date '+%F %T')

  local success failed invalid
  success=$(grep -cE 'Accepted (password|publickey)|session opened for user' "$logfile" || true)
  failed=$(grep -cE 'Failed password|authentication failure' "$logfile" || true)
  invalid=$(grep -cE 'invalid user|user unknown' "$logfile" || true)

  local ips_file
  ips_file=$(mktemp)

  extract_failed_ips "$logfile" "$ips_file" "$use_menu"

  local rows_tsv
  rows_tsv=$(top_rows_tsv "$ips_file" "$top_n")

  local cmd
  if (( use_menu == 1 )); then
    cmd="bash scripts/authlog_report.sh -w -c ${cfg#$ROOT_DIR/} -n $top_n"
  else
    cmd="bash scripts/authlog_report.sh -c ${cfg#$ROOT_DIR/} -l ${logfile#$ROOT_DIR/} -n $top_n"
  fi

  echo "日志文件: $logfile"
  echo "登录成功: $success"
  echo "登录失败:  $failed"
  echo "无效用户:  $invalid"
  echo "可疑 IP Top $top_n:"
  while IFS=$'\t' read -r ip cnt; do
    [[ -n "$ip" ]] || continue
    printf '  %s\t%s\n' "$ip" "$cnt"
  done <<<"$rows_tsv"

  print_report "$logfile" "$ts" "$top_n" "$success" "$failed" "$invalid" "$rows_tsv" "$cmd"

  rm -f "$ips_file"
}

main "$@"