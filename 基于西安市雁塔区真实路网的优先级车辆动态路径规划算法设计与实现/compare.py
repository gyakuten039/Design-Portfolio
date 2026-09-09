# compare.py
import math
import time
import matplotlib.pyplot as plt
import networkx as nx
import random
import heapq
from collections import defaultdict
import sys
import os

# 设置中文字体
plt.rcParams['font.sans-serif'] = ['SimHei', 'Microsoft YaHei', 'DejaVu Sans']
plt.rcParams['axes.unicode_minus'] = False

class AMapNetworkBuilderWithSpeed:
    """包含限速信息的雁塔区路网构建器"""
    
    def __init__(self):
        self.graph = nx.Graph()
        self.node_mapping = {}  # 节点编号映射
        
    def calculate_distance(self, lon1, lat1, lon2, lat2):
        """计算两个坐标点之间的真实距离（米）使用Haversine公式"""
        R = 6371000  # 地球半径，单位米
        
        # 将角度转换为弧度
        lat1_rad = math.radians(lat1)
        lat2_rad = math.radians(lat2)
        delta_lat = math.radians(lat2 - lat1)
        delta_lon = math.radians(lon2 - lon1)
        
        # Haversine公式
        a = (math.sin(delta_lat/2) * math.sin(delta_lat/2) + 
             math.cos(lat1_rad) * math.cos(lat2_rad) * 
             math.sin(delta_lon/2) * math.sin(delta_lon/2))
        c = 2 * math.atan2(math.sqrt(a), math.sqrt(1-a))
        distance = R * c
        
        return round(distance, 1)
    
    def get_speed_limit(self, road_name, road_type):
        """根据道路类型和名称获取限速（km/h）"""
        # 道路名称对应的限速（基于西安市实际情况）
        name_speed_mapping = {
            '长安南路': 60,
            '长安中路': 50,
            '雁塔南路': 50,
            '雁塔西路': 50,
            '翠华路': 40,
            '朱雀大街': 60,
            '电子正街': 40,
            '电子二路': 40,
            '科技路': 60,
            '太乙路': 50,
            '西影路': 50,
        }
        
        # 如果道路名称在映射表中，使用对应的限速
        if road_name in name_speed_mapping:
            return name_speed_mapping[road_name]
        
        # 否则根据道路类型分配限速
        type_speed_mapping = {
            'motorway': 100,     # 高速公路
            'trunk': 80,         # 主干道
            'primary': 60,       # 主要道路
            'secondary': 50,     # 次要道路
            'tertiary': 40,      # 三级道路
            'residential': 30,   # 居民区道路
            'unclassified': 40,  # 未分类道路
        }
        
        if isinstance(road_type, list):
            road_type = road_type[0]
        return type_speed_mapping.get(road_type, 40)
    
    def calculate_base_time(self, length, speed_limit):
        """根据长度和限速计算基准通行时间（秒）"""
        # 速度从km/h转换为m/s
        speed_ms = speed_limit * 1000 / 3600
        base_time = length / speed_ms
        return round(base_time, 1)
    
    def create_yanta_network_with_speed(self):
        """创建包含限速信息的雁塔区主要道路网络"""
        
        # 雁塔区主要地标的真实坐标（从高德地图获取）
        landmarks = {
            # 主要路口和地标
            '大雁塔': (108.9616, 34.2190),
            '小寨': (108.9512, 34.2256),
            '电视塔': (108.9318, 34.2056),
            '交大南门': (108.9949, 34.2475),
            '曲江池': (108.9778, 34.1989),
            '陕历博': (108.9556, 34.2211),
            '西美': (108.9456, 34.2312),
            '明德门': (108.9256, 34.1956),
            '长延堡': (108.9656, 34.2056),
            '翠华路': (108.9556, 34.2156),
            '电子城': (108.9156, 34.2156),
            '南窑头': (108.8856, 34.2356),
            '科技路': (108.9056, 34.2456),
            '师大路': (108.9456, 34.2356),
        }
        
        # 创建节点编号映射
        self.node_mapping = {name: i+1 for i, name in enumerate(landmarks.keys())}
        
        # 道路连接关系和道路类型
        road_connections = [
            # (起点, 终点, 道路名称, 道路类型)
            ('大雁塔', '小寨', '长安南路', 'primary'),
            ('小寨', '电视塔', '长安南路', 'primary'),
            ('小寨', '陕历博', '长安中路', 'primary'),
            ('小寨', '西美', '长安中路', 'primary'),
            ('小寨', '师大路', '长安中路', 'secondary'),
            
            ('大雁塔', '曲江池', '雁塔南路', 'primary'),
            ('大雁塔', '长延堡', '雁塔西路', 'primary'),
            ('长延堡', '翠华路', '雁塔西路', 'primary'),
            
            ('翠华路', '陕历博', '翠华路', 'secondary'),
            ('翠华路', '西美', '翠华路', 'secondary'),
            
            ('电视塔', '明德门', '朱雀大街', 'primary'),
            
            ('电视塔', '电子城', '电子正街', 'secondary'),
            ('电子城', '南窑头', '电子二路', 'tertiary'),
            ('电子城', '科技路', '科技路', 'primary'),
            
            ('交大南门', '长延堡', '太乙路', 'primary'),
            
            ('曲江池', '长延堡', '西影路', 'primary'),
        ]
        
        # 添加节点到图
        for name, (lon, lat) in landmarks.items():
            self.graph.add_node(name, longitude=lon, latitude=lat, 
                              node_id=self.node_mapping[name])
        
        # 添加边并计算长度和基准通行时间
        for start, end, road_name, road_type in road_connections:
            if start in landmarks and end in landmarks:
                start_lon, start_lat = landmarks[start]
                end_lon, end_lat = landmarks[end]
                distance = self.calculate_distance(start_lon, start_lat, end_lon, end_lat)
                
                # 获取限速并计算基准通行时间
                speed_limit = self.get_speed_limit(road_name, road_type)
                base_time = self.calculate_base_time(distance, speed_limit)
                
                self.graph.add_edge(start, end, 
                                  name=road_name, 
                                  road_type=road_type,
                                  length=distance,
                                  speed_limit=speed_limit,
                                  base_time=base_time,
                                  load=0)  # 初始负载为0
        
        print(f"路网构建完成: {len(self.graph.nodes)} 个节点, {len(self.graph.edges)} 条边")
        return self.graph

class BellmanFordAlgorithm:
    """Bellman-Ford算法实现"""
    
    def __init__(self, graph):
        self.graph = graph
    
    def shortest_path(self, source, target):
        """计算从源点到目标点的最短路径"""
        try:
            # 初始化距离和前驱节点
            nodes = list(self.graph.nodes())
            dist = {node: float('inf') for node in nodes}
            prev = {node: None for node in nodes}
            dist[source] = 0
            
            # 松弛操作，执行|V|-1次
            for _ in range(len(nodes) - 1):
                updated = False
                # 对于无向图，每条边需要考虑两个方向
                for u, v, data in self.graph.edges(data=True):
                    # 使用真实长度，不再强制限制
                    weight = data.get('length', 1)
                    
                    # 从u到v
                    if dist[u] + weight < dist[v]:
                        dist[v] = dist[u] + weight
                        prev[v] = u
                        updated = True
                    
                    # 从v到u
                    if dist[v] + weight < dist[u]:
                        dist[u] = dist[v] + weight
                        prev[u] = v
                        updated = True
                
                # 如果这一轮没有更新，提前终止
                if not updated:
                    break
            
            # 重建路径
            if dist[target] == float('inf'):
                return None, float('inf')
            
            path = []
            current = target
            while current is not None:
                path.append(current)
                current = prev[current]
            path.reverse()
            
            return path, dist[target]
        except Exception as e:
            print(f"Bellman-Ford算法出错: {str(e)}")
            return None, float('inf')

class DijkstraAlgorithm:
    """Dijkstra算法实现"""
    
    def __init__(self, graph):
        self.graph = graph
    
    def shortest_path(self, source, target):
        """计算从源点到目标点的最短路径"""
        try:
            # 初始化距离和前驱节点
            nodes = list(self.graph.nodes())
            dist = {node: float('inf') for node in nodes}
            prev = {node: None for node in nodes}
            dist[source] = 0
            
            # 使用优先队列（最小堆）
            heap = [(0, source)]
            visited = set()
            
            while heap:
                current_dist, u = heapq.heappop(heap)
                
                if u in visited:
                    continue
                    
                visited.add(u)
                
                if u == target:
                    break
                    
                for v, data in self.graph[u].items():
                    # 使用真实长度，不再强制限制
                    weight = data.get('length', 1)
                    if dist[u] + weight < dist[v]:
                        dist[v] = dist[u] + weight
                        prev[v] = u
                        heapq.heappush(heap, (dist[v], v))
            
            # 重建路径
            if dist[target] == float('inf'):
                return None, float('inf')
            
            path = []
            current = target
            while current is not None:
                path.append(current)
                current = prev[current]
            path.reverse()
            
            return path, dist[target]
        except Exception as e:
            print(f"Dijkstra算法出错: {str(e)}")
            return None, float('inf')

class AntColonyAlgorithm:
    """蚁群算法实现"""
    
    def __init__(self, graph, num_ants=20, alpha=1.0, beta=2.0, rho=0.5, q=100, max_iter=100):
        self.graph = graph
        self.num_ants = num_ants
        self.alpha = alpha  # 信息素重要程度
        self.beta = beta    # 启发式因子重要程度
        self.rho = rho      # 信息素挥发系数
        self.q = q          # 信息素强度
        self.max_iter = max_iter
        self.pheromone = defaultdict(float)
        self.best_path = None
        self.best_length = float('inf')
        
        # 初始化信息素
        try:
            for u, v in self.graph.edges():
                self.pheromone[(u, v)] = 1.0
                self.pheromone[(v, u)] = 1.0
        except Exception as e:
            print(f"初始化信息素出错: {str(e)}")
    
    def _calculate_distance(self, u, v):
        """计算两个节点之间的距离"""
        try:
            # 使用真实长度，不再强制限制
            return self.graph.edges[u, v].get('length', 1)
        except:
            return 1
    
    def _calculate_visibility(self, u, v):
        """计算启发式信息（距离的倒数）"""
        try:
            distance = self._calculate_distance(u, v)
            return 1.0 / distance if distance > 0 else 1.0
        except:
            return 1.0
    
    def _select_next_node(self, current, visited):
        """选择下一个节点"""
        try:
            neighbors = [n for n in self.graph.neighbors(current) if n not in visited]
            if not neighbors:
                return None
            
            # 计算转移概率
            probabilities = []
            total = 0.0
            
            for neighbor in neighbors:
                pheromone = self.pheromone[(current, neighbor)]
                visibility = self._calculate_visibility(current, neighbor)
                probability = (pheromone ** self.alpha) * (visibility ** self.beta)
                probabilities.append((neighbor, probability))
                total += probability
            
            # 归一化概率
            if total > 0:
                probabilities = [(n, p/total) for n, p in probabilities]
            else:
                # 如果所有概率都是0，均匀选择
                uniform_prob = 1.0 / len(neighbors)
                probabilities = [(n, uniform_prob) for n in neighbors]
            
            # 根据概率选择下一个节点
            r = random.random()
            cumsum = 0.0
            for neighbor, prob in probabilities:
                cumsum += prob
                if r <= cumsum:
                    return neighbor
            
            # 如果由于浮点精度问题没有选中，返回最后一个邻居
            return neighbors[-1]
        except Exception as e:
            print(f"选择下一个节点出错: {str(e)}")
            return None
    
    def _calculate_path_length(self, path):
        """计算路径长度"""
        try:
            length = 0.0
            for i in range(len(path) - 1):
                length += self._calculate_distance(path[i], path[i+1])
            return length
        except:
            return float('inf')
    
    def _update_pheromone(self, all_paths):
        """更新信息素"""
        try:
            # 信息素挥发
            for edge in self.pheromone:
                self.pheromone[edge] *= (1 - self.rho)
            
            # 信息素释放
            for path, length in all_paths:
                if length > 0:
                    pheromone_delta = self.q / length
                    for i in range(len(path) - 1):
                        u, v = path[i], path[i+1]
                        self.pheromone[(u, v)] += pheromone_delta
                        self.pheromone[(v, u)] += pheromone_delta
        except Exception as e:
            print(f"更新信息素出错: {str(e)}")
    
    def shortest_path(self, source, target):
        """计算从源点到目标点的最短路径"""
        try:
            if source == target:
                return [source], 0
            
            for iteration in range(self.max_iter):
                all_paths = []
                
                # 每只蚂蚁构建路径
                for _ in range(self.num_ants):
                    path = [source]
                    visited = {source}
                    current = source
                    
                    # 构建路径
                    while current != target:
                        next_node = self._select_next_node(current, visited)
                        if next_node is None:
                            # 无法继续，重新开始
                            path = [source]
                            visited = {source}
                            current = source
                            continue
                        
                        path.append(next_node)
                        visited.add(next_node)
                        current = next_node
                    
                    # 计算路径长度
                    length = self._calculate_path_length(path)
                    all_paths.append((path, length))
                    
                    # 更新最优解
                    if length < self.best_length:
                        self.best_length = length
                        self.best_path = path.copy()
                
                # 更新信息素
                self._update_pheromone(all_paths)
            
            return self.best_path, self.best_length
        except Exception as e:
            print(f"蚁群算法出错: {str(e)}")
            return None, float('inf')

def compare_algorithms():
    """比较三种算法在西安市雁塔区路网上的运行时间"""
    try:
        print("正在构建西安市雁塔区路网...")
        builder = AMapNetworkBuilderWithSpeed()
        graph = builder.create_yanta_network_with_speed()
        
        # 获取所有节点
        nodes = list(graph.nodes())
        print(f"路网构建完成，共{len(nodes)}个节点，{len(graph.edges)}条边")
        
        # 输出所有边的真实长度信息
        print("\n路网边长度信息:")
        for i, (u, v, data) in enumerate(graph.edges(data=True), 1):
            print(f"  {i:2d}. {u}-{v}: {data.get('length', 0):.1f}m")
        
        # 初始化算法
        bellman_ford = BellmanFordAlgorithm(graph)
        dijkstra = DijkstraAlgorithm(graph)
        ant_colony = AntColonyAlgorithm(graph)
        
        # 准备测试数据 - 选择一些有代表性的起点和终点
        test_cases = [
            ('电子城', '小寨'),
            ('电视塔', '大雁塔'),
            ('交大南门', '曲江池'),
            ('陕历博', '科技路'),
            ('明德门', '南窑头'),
            ('师大路', '西美'),
            ('翠华路', '长延堡')
        ]
        
        # 测试算法性能
        bf_times = []
        dj_times = []
        ac_times = []
        
        bf_lengths = []
        dj_lengths = []
        ac_lengths = []
        
        print("\n开始测试算法性能...")
        for i, (source, target) in enumerate(test_cases):
            print(f"\n测试用例 {i+1}: {source} -> {target}")
            
            # Bellman-Ford算法
            print("  运行 Bellman-Ford 算法...")
            start_time = time.time()
            bf_path, bf_length = bellman_ford.shortest_path(source, target)
            bf_time = time.time() - start_time
            bf_times.append(bf_time)
            bf_lengths.append(bf_length)
            print(f"    Bellman-Ford: 路径={bf_path}, 长度={bf_length:.1f}m, 运行时间={bf_time:.6f}秒")
            
            # Dijkstra算法
            print("  运行 Dijkstra 算法...")
            start_time = time.time()
            dj_path, dj_length = dijkstra.shortest_path(source, target)
            dj_time = time.time() - start_time
            dj_times.append(dj_time)
            dj_lengths.append(dj_length)
            print(f"    Dijkstra: 路径={dj_path}, 长度={dj_length:.1f}m, 运行时间={dj_time:.6f}秒")
            
            # 蚁群算法
            print("  运行蚁群算法...")
            start_time = time.time()
            ac_path, ac_length = ant_colony.shortest_path(source, target)
            ac_time = time.time() - start_time
            ac_times.append(ac_time)
            ac_lengths.append(ac_length)
            print(f"    蚁群算法: 路径={ac_path}, 长度={ac_length:.1f}m, 运行时间={ac_time:.6f}秒")
            
            # 重置蚁群算法的信息素
            ant_colony.pheromone = defaultdict(float)
            for u, v in graph.edges():
                ant_colony.pheromone[(u, v)] = 1.0
                ant_colony.pheromone[(v, u)] = 1.0
            ant_colony.best_path = None
            ant_colony.best_length = float('inf')
        
        # 计算平均运行时间（排除inf值）
        def safe_average(values):
            valid_values = [v for v in values if v != float('inf')]
            return sum(valid_values) / len(valid_values) if valid_values else float('inf')
        
        avg_bf_time = safe_average(bf_times)
        avg_dj_time = safe_average(dj_times)
        avg_ac_time = safe_average(ac_times)
        
        avg_bf_length = safe_average(bf_lengths)
        avg_dj_length = safe_average(dj_lengths)
        avg_ac_length = safe_average(ac_lengths)
        
        print("\n算法性能比较结果:")
        print(f"Bellman-Ford算法: 平均路径长度={avg_bf_length:.1f}m, 平均运行时间={avg_bf_time:.6f}秒")
        print(f"Dijkstra算法: 平均路径长度={avg_dj_length:.1f}m, 平均运行时间={avg_dj_time:.6f}秒")
        print(f"蚁群算法: 平均路径长度={avg_ac_length:.1f}m, 平均运行时间={avg_ac_time:.6f}秒")
        
        # 绘制比较图表
        algorithms = ['Bellman-Ford', 'Dijkstra', '蚁群算法']
        times = [avg_bf_time, avg_dj_time, avg_ac_time]
        lengths = [avg_bf_length, avg_dj_length, avg_ac_length]
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(18, 8))
        
        # 运行时间比较
        bars1 = ax1.bar(algorithms, times, color=['blue', 'green', 'red'])
        ax1.set_title('西安市雁塔区路网 - 三种路径规划算法的平均运行时间比较', fontsize=14)
        ax1.set_ylabel('运行时间 (秒)', fontsize=12)
        ax1.set_xlabel('算法', fontsize=12)
        
        # 在柱状图上添加数值
        for bar, time_val in zip(bars1, times):
            height = bar.get_height()
            if height != float('inf'):
                ax1.text(bar.get_x() + bar.get_width()/2., height + max(times)*0.01,
                        f'{time_val:.6f}', ha='center', va='bottom')
        
        # 路径长度比较
        bars2 = ax2.bar(algorithms, lengths, color=['blue', 'green', 'red'])
        ax2.set_title('西安市雁塔区路网 - 三种路径规划算法的平均路径长度比较', fontsize=14)
        ax2.set_ylabel('路径长度 (米)', fontsize=12)
        ax2.set_xlabel('算法', fontsize=12)
        
        # 在柱状图上添加数值
        for bar, length in zip(bars2, lengths):
            height = bar.get_height()
            if height != float('inf'):
                ax2.text(bar.get_x() + bar.get_width()/2., height + max(lengths)*0.01,
                        f'{length:.1f}', ha='center', va='bottom')
        
        # 添加测试用例信息到图表
        test_info = "测试用例:\n"
        for i, (source, target) in enumerate(test_cases):
            test_info += f"{i+1}. {source} → {target}\n"
        
        # 在图表下方添加测试用例信息
        fig.text(0.02, 0.02, test_info, fontsize=10, 
                verticalalignment='bottom', 
                bbox=dict(boxstyle="round,pad=0.5", facecolor='lightyellow', alpha=0.8))
        
        # 添加网络信息
        network_info = f"网络信息:\n节点数: {len(nodes)}\n边数: {len(graph.edges)}"
        fig.text(0.85, 0.02, network_info, fontsize=10, 
                verticalalignment='bottom', 
                bbox=dict(boxstyle="round,pad=0.5", facecolor='lightblue', alpha=0.8))
        
        plt.tight_layout()
        plt.subplots_adjust(bottom=0.25)  # 调整底部空间以容纳测试用例信息
        
        # 尝试保存图片
        try:
            plt.savefig('yanta_algorithm_comparison_with_info.png', dpi=300, bbox_inches='tight')
            print("\n图表已保存为 yanta_algorithm_comparison_with_info.png")
        except Exception as e:
            print(f"保存图表失败: {str(e)}")
        
        plt.show()
        
        return {
            'avg_times': {'Bellman-Ford': avg_bf_time, 'Dijkstra': avg_dj_time, 'AntColony': avg_ac_time},
            'avg_lengths': {'Bellman-Ford': avg_bf_length, 'Dijkstra': avg_dj_length, 'AntColony': avg_ac_length}
        }
    except Exception as e:
        print(f"比较算法时出错: {str(e)}")
        import traceback
        traceback.print_exc()
        return None

if __name__ == "__main__":
    try:
        # 比较算法性能
        results = compare_algorithms()
        
        if results:
            print("\n结论:")
            print("1. 运行时间方面:")
            if results['avg_times']['Dijkstra'] < results['avg_times']['Bellman-Ford'] and results['avg_times']['Dijkstra'] < results['avg_times']['AntColony']:
                print("   Dijkstra算法最快")
            elif results['avg_times']['Bellman-Ford'] < results['avg_times']['Dijkstra'] and results['avg_times']['Bellman-Ford'] < results['avg_times']['AntColony']:
                print("   Bellman-Ford算法最快")
            else:
                print("   蚁群算法最快")
            
            print("2. 路径质量方面:")
            if results['avg_lengths']['Dijkstra'] < results['avg_lengths']['Bellman-Ford'] and results['avg_lengths']['Dijkstra'] < results['avg_lengths']['AntColony']:
                print("   Dijkstra算法找到的路径最短")
            elif results['avg_lengths']['Bellman-Ford'] < results['avg_lengths']['Dijkstra'] and results['avg_lengths']['Bellman-Ford'] < results['avg_lengths']['AntColony']:
                print("   Bellman-Ford算法找到的路径最短")
            else:
                print("   蚁群算法找到的路径最短")
    except Exception as e:
        print(f"程序运行出错: {str(e)}")
        import traceback
        traceback.print_exc()
