# Dijkstra algorithm.py
import math
import random
import heapq
import matplotlib.pyplot as plt
import networkx as nx
from typing import Dict, List, Tuple, Set, Optional
from datetime import datetime, timedelta
import copy
import os
import time

# 设置中文字体
plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

class Vehicle:
    """车辆类"""
    
    def __init__(self, vehicle_id, priority, start_node, end_node, max_speed, start_time):
        self.vehicle_id = vehicle_id
        self.priority = priority  # 优先级：1-高，2-中，3-低
        self.start_node = start_node
        self.end_node = end_node
        self.max_speed = max_speed  # 车辆最高速度（km/h）
        self.current_speed = 0  # 当前速度
        self.start_time = start_time  # 出发时间
        self.current_time = start_time
        self.current_node = start_node
        self.path = []
        self.travel_log = []  # 行驶记录
        self.status = "waiting"  # waiting, traveling, arrived
        self.current_edge_index = 0  # 当前所在边的索引
        self.edge_progress = 0.0  # 在当前边的进度 (0-1)
        self.edge_start_time = None  # 开始当前边的时间
        self.current_edge = None  # 当前边信息
        self.preference_edges = set()  # 用户偏好规避的边
        
    def get_priority_weight(self):
        """根据优先级获取权重因子"""
        if self.priority == 1:
            return 0.2  # 高优先级权重最小，优先考虑时间
        elif self.priority == 2:
            return 0.5  # 中优先级
        else:
            return 0.8  # 低优先级权重最大，更考虑距离
    
    def add_preference_edge(self, edge):
        """添加偏好规避的边"""
        self.preference_edges.add(edge)
    
    def update_position(self, current_time, network):
        """更新车辆位置"""
        # 检查是否应该开始行驶
        if self.status == "waiting" and current_time >= self.start_time:
            if self.path and len(self.path) > 1:
                self.status = "traveling"
                self.current_time = current_time
                print(f"  {self.vehicle_id}: 开始行驶")
            else:
                print(f"  {self.vehicle_id}: 等待中，但无有效路径")
                return
        
        # 如果车辆已经到达终点
        if self.status == "arrived":
            return
        
        # 如果车辆还未出发或没有路径
        if self.status != "traveling" or not self.path or len(self.path) < 2:
            return
        
        # 获取当前边的信息
        u = self.path[self.current_edge_index]
        v = self.path[self.current_edge_index + 1]
        
        # 获取边数据 - 确保查找两个方向
        edge_data = network.get_edge_data(u, v)
        
        if not edge_data:
            # 如果找不到边数据，这是严重错误，应该停止
            print(f"  {self.vehicle_id}: 严重错误 - 无法找到边 {u}->{v} 的数据")
            print(f"  {self.vehicle_id}: 车辆路径与网络不匹配，停止行驶")
            self.status = "error"
            return
        
        # 如果是新开始一条边，设置开始时间
        if self.edge_start_time is None:
            self.edge_start_time = current_time
            self.current_edge = (u, v)
        
        # 计算边的行驶时间
        length = edge_data.get('length', 300)  # 默认300米
        speed_limit = edge_data.get('speed_limit', 40)
        actual_speed = min(self.max_speed, speed_limit)
        
        # 计算通过这条边需要的时间（秒）
        if actual_speed > 0:
            # 速度从km/h转换为m/s
            speed_mps = actual_speed * 1000 / 3600
            # 时间 = 距离 / 速度
            edge_time = length / speed_mps
        else:
            edge_time = float('inf')
        
        # 计算从开始这条边到现在的时间差
        if self.edge_start_time:
            elapsed = (current_time - self.edge_start_time).total_seconds()
        else:
            elapsed = 0
        
        # 更新边的进度
        if edge_time > 0:
            new_progress = min(1.0, elapsed / edge_time)
            
            # 只有当进度有显著变化时才更新
            if new_progress != self.edge_progress:
                self.edge_progress = new_progress
                
                # 每20%进度或完成时打印一次
                old_percent = int(self.edge_progress * 5)  # 每20%一个区间
                new_percent = int(new_progress * 5)
                if old_percent != new_percent or new_progress >= 1.0:
                    print(f"  {self.vehicle_id}: 边 {u}->{v} 进度: {new_progress:.0%}")
        
        # 如果已经完成这条边
        if elapsed >= edge_time:
            # 完成这条边
            self.current_edge_index += 1
            self.current_node = v
            self.edge_start_time = None
            self.edge_progress = 0.0
            self.current_edge = None
            
            # 记录完成这条边
            self.travel_log.append({
                'from': u,
                'to': v,
                'length': length,
                'speed': actual_speed,
                'speed_limit': speed_limit,
                'start_time': current_time - timedelta(seconds=edge_time),
                'end_time': current_time,
                'travel_time': edge_time
            })
            
            print(f"  {self.vehicle_id}: 完成边 {u}->{v}，长度={length}m，速度={actual_speed}km/h，用时 {edge_time:.1f}秒")
            
            # 检查是否到达终点
            if self.current_node == self.end_node:
                self.status = "arrived"
                self.current_time = current_time
                print(f"  {self.vehicle_id}: 到达终点 {self.end_node}")
        
        # 更新当前时间
        self.current_time = current_time
    
    def get_current_location(self, network):
        """获取车辆当前位置"""
        if not self.path or self.current_edge_index >= len(self.path) - 1:
            return f"{self.current_node}({self.status})"
        
        u = self.path[self.current_edge_index]
        v = self.path[self.current_edge_index + 1]
        
        progress_percent = int(self.edge_progress * 100)
        return f"{u}-{v}({progress_percent}%)"

class NetworkWrapper:
    """网络包装器"""
    def __init__(self, nx_graph):
        self.nodes = {}
        self.edges = {}
        self._nx_graph = nx_graph
        self._extract_data()
        self._ensure_connectivity()
    
    def get_edge_data(self, u, v):
        """获取边数据，支持无向图的双向查找"""
        # 先尝试正向查找
        if (u, v) in self.edges:
            return self.edges[(u, v)]
        # 再尝试反向查找
        if (v, u) in self.edges:
            return self.edges[(v, u)]
        # 如果都找不到，返回None
        return None
    
    def _extract_data(self):
        """从 networkx 图提取数据"""
        # 提取节点数据
        for node, data in self._nx_graph.nodes(data=True):
            self.nodes[node] = {
                'name': node,
                'latitude': data.get('latitude', random.uniform(0, 10)),
                'longitude': data.get('longitude', random.uniform(0, 10)),
                'node_id': data.get('node_id', node)
            }
        
        # 提取边数据 - 确保无向图的双向性
        for u, v, data in self._nx_graph.edges(data=True):
            # 使用真实长度，不再强制限制在100-500米之间
            length = data.get('length', 300)
            
            speed_limit = data.get('speed_limit', 40)
            
            # 为无向图添加双向边数据
            edge_info = {
                'name': data.get('name', f'{u}-{v}'),
                'road_type': data.get('road_type', 'unclassified'),
                'length': length,  # 使用真实长度
                'speed_limit': speed_limit,
                'base_time': data.get('base_time', 60),
                'load': 0,
                'vehicles': []  # 当前在边上的车辆
            }
            
            # 添加两个方向的边
            self.edges[(u, v)] = edge_info.copy()
            self.edges[(v, u)] = edge_info.copy()
            
            print(f"边 {u}-{v}: 长度={length}m, 限速={speed_limit}km/h")
    
    def _ensure_connectivity(self):
        """确保网络连通性"""
        all_nodes = list(self.nodes.keys())
        
        # 确保每个节点至少有一条边
        for node in all_nodes:
            has_edge = False
            for (u, v) in self.edges:
                if u == node or v == node:
                    has_edge = True
                    break
            
            if not has_edge and len(all_nodes) > 1:
                # 找到最近的节点
                other_nodes = [n for n in all_nodes if n != node]
                if other_nodes:
                    nearest = random.choice(other_nodes)
                    edge_info = {
                        'name': f'连接_{node}_{nearest}',
                        'road_type': 'unclassified',
                        'length': 300,  # 连接边使用默认长度300米
                        'speed_limit': 40,
                        'base_time': 60,
                        'load': 0,
                        'vehicles': []
                    }
                    # 添加双向连接
                    self.edges[(node, nearest)] = edge_info.copy()
                    self.edges[(nearest, node)] = edge_info.copy()
    
    def get_all_nodes(self):
        """获取所有节点名称"""
        return list(self.nodes.keys())

class DynamicTrafficSystem:
    """动态交通系统"""
    
    def __init__(self, network):
        self.network = network
        self.ALPHA = 0.3  # 拥堵影响因子
        self.vehicles = {}
        self.current_time = datetime.now()
        self.start_time = self.current_time
        self.time_step = 30  # 30秒，更小的时间步长
        
        # 用于时间步进模拟
        self.simulation_interval = timedelta(seconds=self.time_step)
        self.max_simulation_time = timedelta(minutes=30)  # 最大模拟时间30分钟
    
    def dijkstra(self, start, end, vehicle=None):
        """Dijkstra算法实现，支持优先级和用户偏好"""
        if start == end:
            return {'path': [start], 'speeds': [40]}, 0
        
        nodes = list(self.network.nodes.keys())
        dist = {node: float('inf') for node in nodes}
        prev = {node: None for node in nodes}
        dist[start] = 0
        
        # 使用优先队列
        pq = [(0, start)]
        
        # 构建邻接表，确保包含所有邻居
        adjacency = {}
        for node in nodes:
            adjacency[node] = []
        
        # 填充邻接表
        for (u, v), edge_data in self.network.edges.items():
            adjacency[u].append((v, edge_data))
        
        while pq:
            current_dist, u = heapq.heappop(pq)
            
            # 如果当前距离大于已知距离，跳过
            if current_dist > dist[u]:
                continue
                
            # 遍历所有邻居
            for v, edge_data in adjacency[u]:
                # 检查是否是用户偏好规避的边
                if vehicle and (u, v) in vehicle.preference_edges:
                    continue
                
                length = edge_data.get('length', 100)
                # 考虑当前负载
                load = edge_data.get('load', 0)
                # 增加拥堵影响
                congestion_factor = 1 + self.ALPHA * (load / 5)  # 假设每条路最多容纳5辆车
                
                # 根据车辆优先级计算个性化代价
                if vehicle:
                    weight = vehicle.get_priority_weight()
                    # 个性化代价：距离权重 + 时间权重
                    base_time = length / (edge_data.get('speed_limit', 40) * 1000 / 3600)  # 基准时间
                    cost = length * weight + base_time * congestion_factor * (1 - weight)
                else:
                    cost = length * congestion_factor
                
                if dist[u] + cost < dist[v]:
                    dist[v] = dist[u] + cost
                    prev[v] = u
                    heapq.heappush(pq, (dist[v], v))
        
        # 重建路径
        if dist[end] == float('inf'):
            print(f"警告: 无法找到从 {start} 到 {end} 的路径")
            return {'path': [start, end], 'speeds': [40, 40]}, float('inf')
        
        # 重建路径
        path = []
        current = end
        while current is not None:
            path.append(current)
            current = prev[current]
        path.reverse()
        
        # 为每条边计算建议速度
        speeds = []
        for i in range(len(path) - 1):
            u, v = path[i], path[i+1]
            edge_data = self.network.get_edge_data(u, v)
            if edge_data:
                speeds.append(min(80, edge_data.get('speed_limit', 40)))
            else:
                speeds.append(40)
        
        speeds.append(40)  # 终点速度
        
        return {
            'path': path,
            'speeds': speeds
        }, dist[end]
    
    def add_random_vehicle(self, vehicle_id):
        """添加随机车辆"""
        all_nodes = list(self.network.nodes.keys())
        
        if len(all_nodes) < 2:
            print("错误: 网络节点不足")
            return None
            
        # 随机选择起点和终点
        start_node = random.choice(all_nodes)
        
        # 确保终点不同
        other_nodes = [n for n in all_nodes if n != start_node]
        if not other_nodes:
            print(f"警告: 只有一个节点可用，车辆{vehicle_id}无法创建")
            return None
        end_node = random.choice(other_nodes)
        
        # 随机生成优先级
        priority = random.randint(1, 3)
        
        # 随机生成速度
        max_speed = random.randint(40, 60)  # 限制速度范围
        
        # 随机生成出发时间（在当前时间后的0-2分钟内）
        minutes_offset = random.randint(0, 2)
        start_time = self.current_time + timedelta(minutes=minutes_offset)
        
        vehicle = Vehicle(vehicle_id, priority, start_node, end_node, max_speed, start_time)
        self.vehicles[vehicle_id] = vehicle
        
        print(f"车辆 {vehicle_id}:")
        print(f"  起点: {start_node} → 终点: {end_node}")
        print(f"  优先级: {self.get_priority_name(priority)}")
        print(f"  速度: {max_speed} km/h")
        print(f"  出发时间: {start_time.strftime('%H:%M:%S')}")
        
        return vehicle
    
    def add_manual_vehicle(self, vehicle_id):
        """手动添加车辆"""
        all_nodes = self.network.get_all_nodes()
        
        if len(all_nodes) < 2:
            print("错误: 网络节点不足")
            return None
        
        print(f"\n为车辆 {vehicle_id} 输入信息:")
        print("可用的节点:", ", ".join(all_nodes[:10]))
        if len(all_nodes) > 10:
            print(f"  ... 还有 {len(all_nodes)-10} 个节点")
        
        # 输入起点
        while True:
            start_node = input("  输入起点: ").strip()
            if start_node in all_nodes:
                break
            print(f"  错误: 节点 '{start_node}' 不存在，请重新输入")
        
        # 输入终点
        while True:
            end_node = input("  输入终点: ").strip()
            if end_node in all_nodes:
                if end_node != start_node:
                    break
                else:
                    print("  错误: 终点不能与起点相同")
            else:
                print(f"  错误: 节点 '{end_node}' 不存在，请重新输入")
        
        # 输入优先级
        while True:
            try:
                priority_input = input("  输入优先级 (1-高, 2-中, 3-低, 默认2): ").strip()
                if not priority_input:
                    priority = 2
                    break
                priority = int(priority_input)
                if 1 <= priority <= 3:
                    break
                print("  错误: 请输入1、2或3")
            except ValueError:
                print("  错误: 请输入数字")
        
        # 输入速度
        while True:
            try:
                speed_input = input("  输入最高速度 (km/h, 30-60, 默认50): ").strip()
                if not speed_input:
                    max_speed = 50
                    break
                max_speed = int(speed_input)
                if 30 <= max_speed <= 60:
                    break
                print("  错误: 请输入30-60之间的数字")
            except ValueError:
                print("  错误: 请输入数字")
        
        # 输入出发时间偏移
        while True:
            try:
                time_input = input("  输入出发时间偏移(分钟, 0-5, 默认0): ").strip()
                if not time_input:
                    minutes_offset = 0
                    break
                minutes_offset = int(time_input)
                if 0 <= minutes_offset <= 5:
                    break
                print("  错误: 请输入0-5之间的数字")
            except ValueError:
                print("  错误: 请输入数字")
        
        start_time = self.current_time + timedelta(minutes=minutes_offset)
        
        vehicle = Vehicle(vehicle_id, priority, start_node, end_node, max_speed, start_time)
        
        # 询问是否需要设置偏好规避路段
        pref_input = input("  是否需要设置偏好规避路段？(y/n, 默认n): ").strip().lower()
        if pref_input == 'y':
            print("  可用的边:")
            edge_list = list(self.network.edges.keys())
            for i, (u, v) in enumerate(edge_list[:10]):  # 只显示前10条边
                print(f"    {i+1}. {u}-{v}")
            if len(edge_list) > 10:
                print(f"    ... 还有 {len(edge_list)-10} 条边")
            
            while True:
                edge_input = input("  输入要规避的边编号（用逗号分隔，0结束）: ").strip()
                if edge_input == '0':
                    break
                try:
                    indices = [int(x.strip()) for x in edge_input.split(',')]
                    for idx in indices:
                        if 1 <= idx <= len(edge_list):
                            u, v = edge_list[idx-1]
                            vehicle.add_preference_edge((u, v))
                            print(f"    已添加规避边: {u}-{v}")
                        else:
                            print(f"    编号 {idx} 无效")
                except ValueError:
                    print("  输入格式错误，请输入数字编号")
        
        self.vehicles[vehicle_id] = vehicle
        
        print(f"\n车辆 {vehicle_id} 添加成功:")
        print(f"  起点: {start_node} → 终点: {end_node}")
        print(f"  优先级: {self.get_priority_name(priority)}")
        print(f"  速度: {max_speed} km/h")
        print(f"  出发时间: {start_time.strftime('%H:%M:%S')}")
        if vehicle.preference_edges:
            print(f"  规避路段: {', '.join([f'{u}-{v}' for u, v in vehicle.preference_edges])}")
        
        return vehicle
    
    def get_priority_name(self, priority):
        """获取优先级名称"""
        names = {1: "高优先级", 2: "中优先级", 3: "低优先级"}
        return names.get(priority, f"优先级{priority}")
    
    def plan_route(self, vehicle_id):
        """为车辆规划路径"""
        if vehicle_id not in self.vehicles:
            return None
            
        vehicle = self.vehicles[vehicle_id]
        
        print(f"\n为车辆 {vehicle_id} 规划路径...")
        
        # 使用Dijkstra算法
        path_info, cost = self.dijkstra(vehicle.start_node, vehicle.end_node, vehicle)
        
        if path_info and 'path' in path_info:
            vehicle.path = path_info['path']
            vehicle.current_node = vehicle.start_node
            
            print(f"  找到路径: {' -> '.join(vehicle.path)}")
            print(f"  路径长度: {len(vehicle.path)}个节点")
            print(f"  总代价: {cost:.2f}")
            
            # 估算到达时间
            total_distance = 0
            for i in range(len(vehicle.path) - 1):
                u, v = vehicle.path[i], vehicle.path[i+1]
                edge_data = self.network.get_edge_data(u, v)
                if edge_data:
                    total_distance += edge_data['length']
                else:
                    total_distance += 300  # 默认长度
            
            avg_speed = 50  # 平均速度 km/h
            if total_distance > 0:
                est_time = (total_distance / 1000) / avg_speed * 3600  # 秒
                est_arrival = vehicle.start_time + timedelta(seconds=est_time)
                print(f"  预计到达: {est_arrival.strftime('%H:%M:%S')} (距离: {total_distance}m, 约{est_time/60:.1f}分钟)")
            
            return path_info
        else:
            print(f"  无法找到从 {vehicle.start_node} 到 {vehicle.end_node} 的路径")
            # 至少设置一个简单的路径
            vehicle.path = [vehicle.start_node, vehicle.end_node]
            vehicle.current_node = vehicle.start_node
            return None
    
    def simulate_random_vehicles(self, num_vehicles):
        """模拟随机生成的车辆"""
        print(f"\n随机生成 {num_vehicles} 辆车辆...")
        
        for i in range(num_vehicles):
            vehicle_id = f"R{i+1:03d}"  # R表示随机
            print(f"\n[{i+1}/{num_vehicles}] 随机车辆 {vehicle_id}")
            
            vehicle = self.add_random_vehicle(vehicle_id)
            if not vehicle:
                print(f"  车辆创建失败，跳过")
                continue
                
            self.plan_route(vehicle_id)
        
        print(f"\n随机车辆生成完成")
        return len(self.vehicles)
    
    def simulate_manual_vehicles(self, num_vehicles):
        """手动输入车辆信息"""
        print(f"\n手动输入 {num_vehicles} 辆车辆信息...")
        
        for i in range(num_vehicles):
            vehicle_id = f"M{i+1:03d}"  # M表示手动
            print(f"\n[{i+1}/{num_vehicles}] 手动输入车辆 {vehicle_id}")
            
            vehicle = self.add_manual_vehicle(vehicle_id)
            if not vehicle:
                print(f"  车辆创建失败，跳过")
                continue
                
            self.plan_route(vehicle_id)
        
        print(f"\n手动输入车辆完成")
        return len(self.vehicles)
    
    def run_time_step_simulation(self):
        """运行时间步进模拟"""
        print("\n" + "="*60)
        print("开始时间步进模拟")
        print(f"时间步长: {self.time_step}秒")
        print("="*60)
        
        # 初始化可视化
        try:
            from yanta import AMapNetworkBuilderWithSpeed
            builder = AMapNetworkBuilderWithSpeed()
            network_graph = builder.create_yanta_network_with_speed()
        except Exception as e:
            print(f"初始化可视化失败: {e}")
            builder = None
        
        # 输出所有车辆初始信息
        print(f"\n初始状态 - 当前时间: {self.current_time.strftime('%H:%M:%S')}")
        print(f"总车辆数: {len(self.vehicles)}")
        
        for vehicle_id, vehicle in self.vehicles.items():
            print(f"  {vehicle_id}: {vehicle.start_node}->{vehicle.end_node}, "
                  f"状态={vehicle.status}, 出发时间={vehicle.start_time.strftime('%H:%M:%S')}")
        
        simulation_time = self.current_time
        step_count = 0
        all_arrived = False
        
        # 初始可视化
        if builder:
            vehicles_data = []
            for vehicle_id, vehicle in self.vehicles.items():
                vehicles_data.append({
                    'vehicle_id': vehicle_id,
                    'current_node': vehicle.current_node,
                    'next_node': vehicle.path[vehicle.current_edge_index + 1] if vehicle.status == "traveling" and vehicle.current_edge_index < len(vehicle.path) - 1 else None,
                    'edge_progress': vehicle.edge_progress,
                    'status': vehicle.status,
                    'path': vehicle.path,
                    'color': 'green'  # Dijkstra使用绿色
                })
            
            print("\n显示初始车辆位置...")
            builder.visualize_vehicles_realtime(vehicles_data, "Dijkstra算法 - 车辆实时位置", simulation_time, blocking=True)
        
        while not all_arrived and (simulation_time - self.start_time) < self.max_simulation_time:
            step_count += 1
            simulation_time += self.simulation_interval
            
            print(f"\n[时间: {simulation_time.strftime('%H:%M:%S')}] 步骤 {step_count}")
            
            # 更新所有车辆位置
            for vehicle in self.vehicles.values():
                vehicle.update_position(simulation_time, self.network)
            
            # 检查是否所有车辆都到达
            arrived_count = sum(1 for v in self.vehicles.values() if v.status == "arrived")
            traveling_count = sum(1 for v in self.vehicles.values() if v.status == "traveling")
            waiting_count = sum(1 for v in self.vehicles.values() if v.status == "waiting")
            error_count = sum(1 for v in self.vehicles.values() if v.status == "error")
            total_count = len(self.vehicles)
            
            # 输出当前状态
            print(f"车辆状态: 已到达={arrived_count}, 行驶中={traveling_count}, 等待中={waiting_count}, 错误={error_count}")
            
            # 显示行驶中车辆的详细信息
            if traveling_count > 0:
                print(f"行驶中车辆:")
                for vehicle_id, vehicle in self.vehicles.items():
                    if vehicle.status == "traveling":
                        location = vehicle.get_current_location(self.network)
                        # 计算总体进度
                        if vehicle.path and len(vehicle.path) > 1:
                            total_edges = len(vehicle.path) - 1
                            completed_edges = vehicle.current_edge_index
                            edge_progress = vehicle.edge_progress
                            overall_progress = (completed_edges + edge_progress) / total_edges * 100
                        else:
                            overall_progress = 0
                        print(f"  {vehicle_id}: {location}, 总体进度={overall_progress:.1f}%")
            
            # 显示错误车辆
            if error_count > 0:
                print(f"错误车辆:")
                for vehicle_id, vehicle in self.vehicles.items():
                    if vehicle.status == "error":
                        print(f"  {vehicle_id}: {vehicle.current_node} -> {vehicle.end_node}")
            
            # 实时可视化更新
            if builder:
                vehicles_data = []
                for vehicle_id, vehicle in self.vehicles.items():
                    next_node = None
                    if vehicle.status == "traveling" and vehicle.current_edge_index < len(vehicle.path) - 1:
                        next_node = vehicle.path[vehicle.current_edge_index + 1]
                    
                    vehicles_data.append({
                        'vehicle_id': vehicle_id,
                        'current_node': vehicle.current_node,
                        'next_node': next_node,
                        'edge_progress': vehicle.edge_progress,
                        'status': vehicle.status,
                        'path': vehicle.path,
                        'color': 'green'  # Dijkstra使用绿色
                    })
                
                print(f"\n显示第 {step_count} 步的车辆位置...")
                builder.visualize_vehicles_realtime(vehicles_data, "Dijkstra算法 - 车辆实时位置", simulation_time, blocking=True)
            
            # 检查是否所有车辆都到达或出错
            all_arrived = (arrived_count + error_count == total_count)
            
            if all_arrived:
                print(f"\n所有车辆都已到达终点!")
                break
            
            # 每10步输出简要统计
            if step_count % 10 == 0:
                print(f"\n[统计] 已模拟: {step_count}步, 已到达: {arrived_count}/{total_count}")
        
        # 模拟结束
        print("\n" + "="*60)
        print("模拟结束!")
        elapsed_time = (simulation_time - self.start_time).total_seconds()
        print(f"总模拟时间: {elapsed_time/60:.1f}分钟")
        print(f"总步数: {step_count}")
        print("="*60)
        
        # 最终可视化
        if builder:
            vehicles_data = []
            for vehicle_id, vehicle in self.vehicles.items():
                next_node = None
                if vehicle.status == "traveling" and vehicle.current_edge_index < len(vehicle.path) - 1:
                    next_node = vehicle.path[vehicle.current_edge_index + 1]
                
                vehicles_data.append({
                    'vehicle_id': vehicle_id,
                    'current_node': vehicle.current_node,
                    'next_node': next_node,
                    'edge_progress': vehicle.edge_progress,
                    'status': vehicle.status,
                    'path': vehicle.path,
                    'color': 'green'
                })
            
            print("\n显示最终车辆位置...")
            builder.visualize_vehicles_realtime(vehicles_data, "Dijkstra算法 - 最终车辆位置", simulation_time, blocking=True)
    
        self.print_final_summary()
    
    def print_final_summary(self):
        """打印最终摘要"""
        print("\n" + "="*60)
        print("最终系统摘要")
        print("="*60)
        
        total_vehicles = len(self.vehicles)
        arrived_count = sum(1 for v in self.vehicles.values() if v.status == "arrived")
        error_count = sum(1 for v in self.vehicles.values() if v.status == "error")
        
        print(f"车辆总数: {total_vehicles}")
        if total_vehicles > 0:
            print(f"成功到达: {arrived_count} ({arrived_count/total_vehicles*100:.1f}%)")
            print(f"出错: {error_count} ({error_count/total_vehicles*100:.1f}%)")
        else:
            print(f"成功到达: 0 (0.0%)")
        
        # 统计不同类型车辆
        random_count = sum(1 for v_id in self.vehicles if v_id.startswith('R'))
        manual_count = sum(1 for v_id in self.vehicles if v_id.startswith('M'))
        
        if random_count > 0:
            print(f"随机生成车辆: {random_count}辆")
        if manual_count > 0:
            print(f"手动输入车辆: {manual_count}辆")
        
        # 统计优先级
        priorities = {1: 0, 2: 0, 3: 0}
        total_distance = 0
        total_time = 0
        
        for vehicle in self.vehicles.values():
            if vehicle.status == "arrived":
                priorities[vehicle.priority] += 1
                for log in vehicle.travel_log:
                    total_distance += log['length']
                    total_time += log['travel_time']
        
        print(f"\n到达车辆优先级分布:")
        if arrived_count > 0:
            for p in [1, 2, 3]:
                count = priorities[p]
                if count > 0:
                    print(f"  {self.get_priority_name(p)}: {count}辆 ({count/arrived_count*100:.1f}%)")
        else:
            print("  没有车辆到达")
        
        if total_distance > 0 and total_time > 0:
            print(f"\n总行驶距离: {total_distance:.0f}米")
            print(f"总行驶时间: {total_time:.0f}秒 ({total_time/60:.1f}分钟)")
            avg_speed = (total_distance / 1000) / (total_time / 3600)  # km/h
            print(f"平均速度: {avg_speed:.1f} km/h")
        
        # 输出未到达车辆信息
        not_arrived = [v for v in self.vehicles.values() if v.status != "arrived"]
        if not_arrived:
            print(f"\n未到达车辆:")
            for vehicle in not_arrived:
                print(f"  {vehicle.vehicle_id}: {vehicle.start_node}->{vehicle.end_node}, 状态={vehicle.status}")
                if vehicle.status == "traveling":
                    print(f"    当前位置: {vehicle.current_node}, 完成边: {vehicle.current_edge_index}/{len(vehicle.path)-1}")

# 主函数
def main():
    print("="*60)
    print("车辆路径规划系统 - Dijkstra算法实现")
    print("雁塔区交通网络 - 优先级车辆调度")
    print("="*60)
    
    try:
        # 尝试导入网络
        try:
            from yanta import AMapNetworkBuilderWithSpeed
            
            print("\n正在构建雁塔区交通网络...")
            builder = AMapNetworkBuilderWithSpeed()
            nx_graph = builder.create_yanta_network_with_speed()
            
            if not nx_graph or len(nx_graph.nodes) == 0:
                print("网络创建失败，使用测试网络")
                raise ImportError("网络创建失败")
            
            print(f"✓ 网络创建成功")
            print(f"  节点数: {len(nx_graph.nodes)}")
            print(f"  边数: {len(nx_graph.edges)}")
            
            # 显示可用节点
            nodes = list(nx_graph.nodes())
            print(f"\n可用节点 (前10个): {', '.join(nodes[:10])}")
            if len(nodes) > 10:
                print(f"  共 {len(nodes)} 个节点")
                
        except ImportError:
            print("\n使用测试网络...")
            # 创建测试网络
            nx_graph = nx.Graph()
            
            # 创建一些测试节点
            nodes = ["小寨", "电视塔", "电子城", "陕历博", "大雁塔", "曲江池", "翠华路", "长延堡", "交大南门"]
            
            for node in nodes:
                nx_graph.add_node(node)
            
            # 添加边，确保长度合理（100-500米）
            edges = [
                ("小寨", "电视塔", 400),
                ("电视塔", "电子城", 350),
                ("小寨", "陕历博", 300),
                ("陕历博", "大雁塔", 250),
                ("大雁塔", "曲江池", 200),
                ("电视塔", "陕历博", 450),
                ("翠华路", "长延堡", 320),
                ("长延堡", "交大南门", 280),
                ("小寨", "翠华路", 380)
            ]
            
            for u, v, length in edges:
                nx_graph.add_edge(u, v,
                                 length=length,
                                 speed_limit=random.randint(40, 60))
            
            print(f"✓ 测试网络创建成功")
            print(f"  节点数: {len(nx_graph.nodes)}")
            print(f"  边数: {len(nx_graph.edges)}")
            print(f"\n可用节点: {', '.join(nodes)}")
        
        # 创建系统
        network = NetworkWrapper(nx_graph)
        system = DynamicTrafficSystem(network)
        
        # 选择车辆生成方式
        print("\n" + "="*40)
        print("选择车辆生成方式:")
        print("1. 随机生成车辆")
        print("2. 手动输入车辆信息")
        print("3. 两种方式混合")
        print("="*40)
        
        while True:
            try:
                choice_input = input("\n请选择 (1/2/3, 默认1): ").strip()
                if not choice_input:
                    choice = 1
                    break
                choice = int(choice_input)
                if 1 <= choice <= 3:
                    break
                print("错误: 请输入1、2或3")
            except ValueError:
                print("错误: 请输入数字")
        
        # 根据选择添加车辆
        if choice == 1:
            # 随机生成车辆
            while True:
                try:
                    num_input = input("\n输入随机生成的车辆数量 (1-10, 默认5): ").strip()
                    if not num_input:
                        num_vehicles = 5
                        break
                    num_vehicles = int(num_input)
                    if 1 <= num_vehicles <= 10:
                        break
                    print("错误: 请输入1-10之间的数字")
                except ValueError:
                    print("错误: 请输入数字")
            
            system.simulate_random_vehicles(num_vehicles)
            
        elif choice == 2:
            # 手动输入车辆
            while True:
                try:
                    num_input = input("\n输入要手动创建的车辆数量 (1-5, 默认3): ").strip()
                    if not num_input:
                        num_vehicles = 3
                        break
                    num_vehicles = int(num_input)
                    if 1 <= num_vehicles <= 5:
                        break
                    print("错误: 请输入1-5之间的数字")
                except ValueError:
                    print("错误: 请输入数字")
            
            system.simulate_manual_vehicles(num_vehicles)
            
        else:
            # 混合方式
            while True:
                try:
                    random_input = input("\n输入随机生成的车辆数量 (0-8, 默认3): ").strip()
                    if not random_input:
                        random_count = 3
                        break
                    random_count = int(random_input)
                    if 0 <= random_count <= 8:
                        break
                    print("错误: 请输入0-8之间的数字")
                except ValueError:
                    print("错误: 请输入数字")
            
            while True:
                try:
                    manual_input = input("输入手动输入的车辆数量 (0-3, 默认2): ").strip()
                    if not manual_input:
                        manual_count = 2
                        break
                    manual_count = int(manual_input)
                    if 0 <= manual_count <= 3:
                        break
                    print("错误: 请输入0-3之间的数字")
                except ValueError:
                    print("错误: 请输入数字")
            
            if random_count > 0:
                system.simulate_random_vehicles(random_count)
            
            if manual_count > 0:
                system.simulate_manual_vehicles(manual_count)
        
        if not system.vehicles:
            print("\n错误: 没有成功创建任何车辆")
            return
        
        # 设置时间步长
        print("\n" + "="*40)
        print("时间步进模拟设置")
        print("="*40)
        
        while True:
            try:
                step_input = input("\n输入时间步长 (秒, 10-120, 默认30): ").strip()
                if not step_input:
                    step_seconds = 30
                    break
                step_seconds = int(step_input)
                if 10 <= step_seconds <= 120:
                    break
                print("错误: 请输入10-120之间的数字")
            except ValueError:
                print("错误: 请输入数字")
        
        system.time_step = step_seconds
        system.simulation_interval = timedelta(seconds=system.time_step)
        
        # 开始时间步进模拟
        print(f"\n开始模拟，每 {step_seconds} 秒输出一次状态...")
        input("按Enter键开始模拟...")
        
        system.run_time_step_simulation()
        
        print("\n" + "="*60)
        print("程序执行完成!")
        print("="*60)
        
    except KeyboardInterrupt:
        print("\n\n程序被用户中断")
    except Exception as e:
        print(f"\n程序运行出错: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()
