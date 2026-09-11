#!/bin/bash
# 设置要备份的源目录路径（使用 WSL 挂载的 Windows 路径）
# 这是你的 "linux大作业" 文件夹路径
SOURCE_DIR="/mnt/e/linux大作业"
# 设置备份文件存放的目标目录
BACKUP_DIR="/mnt/e/linux大作业/backups"
# 获取当前日期，格式为年月日，如20260409
DATE=$(date +%y%m%d)
# 组合出完整的备份文件名
BACKUP_FILE="$BACKUP_DIR/backup_$DATE.tar.gz"
# 如果备份目录不存在，创建备份目录
mkdir -p $BACKUP_DIR
# 打包压缩
tar -czf $BACKUP_FILE $SOURCE_DIR
# 判断备份是否成功并输出相应信息
if [ $? -eq 0 ]; then
echo "Backup success: $BACKUP_FILE"
else
echo "Backup failed"
fi
# 删除30天前的旧备份文件
find $BACKUP_DIR -name "backup_*.tar.gz" -mtime +30 -delete
