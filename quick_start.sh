#!/bin/bash

# 快速启动脚本 - 智能家居沙盘系统API服务
# 使用方法: ./quick_start.sh

echo "=== 智能家居沙盘 - 快速启动 ==="

# 颜色定义
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

# 检查Docker服务
echo -e "${YELLOW}检查Docker服务...${NC}"
if ! sudo systemctl is-active --quiet docker; then
    echo -e "${RED}Docker服务未运行，正在启动...${NC}"
    sudo systemctl start docker
    sleep 2
    if ! sudo systemctl is-active --quiet docker; then
        echo -e "${RED}Docker服务启动失败，请检查Docker安装${NC}"
        exit 1
    fi
fi
echo -e "${GREEN}✓ Docker服务运行正常${NC}"

# 检查Mosquitto容器
echo -e "${YELLOW}检查Mosquitto容器...${NC}"
if ! docker ps --format "table {{.Names}}" | grep -q "^mosquitto$"; then
    echo -e "${RED}Mosquitto容器未运行，正在启动...${NC}"
    if docker ps -a --format "table {{.Names}}" | grep -q "^mosquitto$"; then
        docker start mosquitto
    else
        echo -e "${RED}Mosquitto容器不存在，请先运行完整部署脚本${NC}"
        echo "运行命令: ./deploy_raspberry_pi.sh"
        exit 1
    fi
    sleep 2
fi
echo -e "${GREEN}✓ Mosquitto容器运行正常${NC}"

# 检查conda环境
echo -e "${YELLOW}检查conda环境...${NC}"
if ! conda env list | grep -q "^iot "; then
    echo -e "${RED}conda环境 'iot' 不存在，正在创建...${NC}"
    conda create -n iot python=3.9 -y
fi
echo -e "${GREEN}✓ conda环境 'iot' 已准备${NC}"

# 激活conda环境并启动API服务
echo -e "${YELLOW}启动API服务...${NC}"
echo -e "${GREEN}正在激活conda环境并启动服务...${NC}"

# 激活conda环境
source $(conda info --base)/etc/profile.d/conda.sh
conda activate iot

# 检查依赖
if ! python -c "import fastapi, uvicorn, paho.mqtt" 2>/dev/null; then
    echo -e "${YELLOW}安装Python依赖...${NC}"
    if [ -f "smart_home_api/requirements.txt" ]; then
        pip install -r smart_home_api/requirements.txt
    else
        pip install fastapi uvicorn[standard] paho-mqtt
    fi
fi

echo -e "${GREEN}✓ 所有服务准备就绪${NC}"
echo ""
echo "=== 服务状态 ==="
echo "Docker服务: $(sudo systemctl is-active docker)"
echo "Mosquitto容器: $(docker ps --format "table {{.Names}}" | grep mosquitto | wc -l)个运行中"
echo "conda环境: iot"
echo ""
echo "=== 启动API服务 ==="
echo "API服务将在 http://0.0.0.0:8000 启动"
echo "API文档地址: http://0.0.0.0:8000/docs"
echo "按 Ctrl+C 停止服务"
echo ""

# 启动API服务
uvicorn smart_home_api.main:app --reload --host 0.0.0.0
