#!/bin/bash

# ==============================================
# Linux大作业：批量清理空目录/临时文件 + HTTP服务检测
# 环境：Ubuntu 桌面版
# 功能：1.清理tmp文件 2.删除空目录 3.检测网址/IP服务状态
# ==============================================

# ===================== 全局变量 =====================
VERSION="v1.0"
# 临时文件后缀
TEMP_SUFFIX="xx1 xx2"
# HTTP检测超时时间
TIMEOUT=5

# ===================== 函数1：帮助说明 =====================
usage() {
    echo "===== 脚本使用方法 ====="
    echo "1. 清理空目录：        $0 clean_dir 目标目录"
    echo "   示例：$0 clean_dir ~/test"
    echo "2. 清理临时文件：      $0 clean_tmp 目标目录"
    echo "   示例：$0 clean_tmp ~/test"
    echo "3. HTTP服务检测：      $0 http 网址/IP"
    echo "   示例：$0 http www.baidu.com"
    echo "4. 查看版本：         $0 version"
    echo "========================"
}

# ===================== 函数2：批量删除空目录 =====================
clean_empty_dir() {
    local target_dir=$1

    # 判断目录是否存在
    if [ ! -d "$target_dir" ]; then
        echo "错误：目录 $target_dir 不存在！"
        exit 1
    fi

    echo "===== 开始扫描并删除空目录：$target_dir ====="
    echo "扫描时间：$(date)"

    # 循环：查找空目录并删除
    local count=0
    find "$target_dir" -type d -empty | while read -r dir; do
        echo "删除空目录：$dir"
        rmdir "$dir"
        count=$((count + 1))
    done

    echo "===== 清理完成！共删除空目录：$count 个 ====="
}

# ===================== 函数3：批量清理临时文件 =====================
clean_temp_files() {
    local target_dir=$1

    if [ ! -d "$target_dir" ]; then
        echo "错误：目录 $target_dir 不存在！"
        exit 1
    fi

    echo "===== 开始清理临时文件：$target_dir ====="
    echo "清理文件类型：$TEMP_SUFFIX"
    echo "扫描时间：$(date)"

    local count=0
    # 循环删除各类临时文件
    for suffix in $TEMP_SUFFIX; do
        find "$target_dir" -type f -name "*.$suffix" | while read -r file; do
            echo "删除临时文件：$file"
            rm -f "$file"
            count=$((count + 1))
        done
    done

    echo "===== 清理完成！共删除临时文件：$count 个 ====="
}


# ===================== 主程序：参数处理 =====================
if [ $# -lt 1 ]; then
    usage
    exit 1
fi

mode=$1

case $mode in
    clean_dir)
        [ $# -ne 2 ] && { echo "参数错误！"; usage; exit 1; }
        clean_empty_dir "$2"
        ;;
    clean_tmp)
        [ $# -ne 2 ] && { echo "参数错误！"; usage; exit 1; }
        clean_temp_files "$2"
        ;;
    version)
        echo "脚本版本：$VERSION"
        ;;
    *)
        usage
        ;;
esac
