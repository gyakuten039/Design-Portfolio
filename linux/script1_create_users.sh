#!/bin/bash
# 询问用户前缀
echo "Enter username prefix:"
read PREFIX
# 询问创建数量
echo "Enter number of users to create:"
read COUNT
# 询问起始编号
echo "Enter starting number (default 1):"
read START
# 如果没输入起始编号，默认为1
if [ -z "$START" ]; then
START=1
fi
# 计算结束编号
END=$((START + COUNT - 1))
# 循环创建用户
for i in $(seq $START $END)
do
USERNAME="$PREFIX$i"
# 创建用户，-m 表示同时创建家目录
sudo useradd -m "$USERNAME"
# 检查是否创建成功
if [ $? -eq 0 ]; then
echo "Created user: $USERNAME"
else
echo "Failed to create user: $USERNAME"
fi
done
echo "User creation completed."
