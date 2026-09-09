import math
import matplotlib.pyplot as plt
import networkx as nx
from typing import Dict, List, Tuple, Set

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
    
    def visualize_paths(self, paths_data, title="路径规划结果"):
        """
        可视化多个路径规划结果
        
        参数:
        paths_data: 路径数据列表，每个元素是包含以下键的字典:
            - 'algorithm': 算法名称
            - 'vehicle_id': 车辆ID
            - 'path': 路径节点列表
            - 'color': 路径颜色
        title: 图表标题
        """
        if len(self.graph.nodes) == 0:
            print("没有可可视化的图数据")
            return
        
        # 创建图形
        plt.figure(figsize=(16, 12))
        
        # 获取节点位置
        pos = {node: (data['longitude'], data['latitude']) 
               for node, data in self.graph.nodes(data=True)}
        
        # 绘制边 - 统一使用灰色
        for u, v, data in self.graph.edges(data=True):
            x1, y1 = pos[u]
            x2, y2 = pos[v]
            
            # 绘制道路线（统一灰色）
            plt.plot([x1, x2], [y1, y2], 'gray', alpha=0.5, linewidth=2, zorder=1)
        
        # 绘制节点 - 显示节点名称
        for node, (x, y) in pos.items():
            # 绘制节点
            plt.scatter(x, y, color='red', s=150, zorder=5, alpha=0.9, 
                       edgecolors='darkred', linewidth=2)
            
            # 显示节点名称
            plt.text(x, y, node, 
                    fontsize=10, ha='center', va='center', 
                    fontweight='bold', color='white', zorder=6)
        
        # 为每条路径绘制不同颜色的线
        for path_info in paths_data:
            path = path_info['path']
            algorithm = path_info['algorithm']
            vehicle_id = path_info['vehicle_id']
            color = path_info['color']
            
            # 绘制路径
            for i in range(len(path) - 1):
                u, v = path[i], path[i+1]
                x1, y1 = pos[u]
                x2, y2 = pos[v]
                
                # 绘制路径线
                plt.plot([x1, x2], [y1, y2], color=color, alpha=0.8, 
                        linewidth=3, zorder=2,
                        label=f"{algorithm} - {vehicle_id}")
                
                # 在路径中点添加小标记
                mid_x, mid_y = (x1 + x2) / 2, (y1 + y2) / 2
                plt.scatter(mid_x, mid_y, color=color, s=50, zorder=4)
        
        # 设置地图显示范围
        all_lons = [pos[node][0] for node in self.graph.nodes()]
        all_lats = [pos[node][1] for node in self.graph.nodes()]
        
        lon_margin = (max(all_lons) - min(all_lons)) * 0.15
        lat_margin = (max(all_lats) - min(all_lats)) * 0.15
        
        plt.xlim(min(all_lons) - lon_margin, max(all_lons) + lon_margin)
        plt.ylim(min(all_lats) - lat_margin, max(all_lats) + lat_margin)
        
        plt.title(title, fontsize=18, fontweight='bold', pad=20)
        plt.xlabel('经度', fontsize=12)
        plt.ylabel('纬度', fontsize=12)
        plt.grid(True, alpha=0.3)
        
        # 添加图例
        handles, labels = plt.gca().get_legend_handles_labels()
        by_label = dict(zip(labels, handles))
        plt.legend(by_label.values(), by_label.keys(), loc='upper right')
        
        plt.tight_layout()
        plt.show()
    
    def visualize_vehicles_realtime(self, vehicles_data, title="车辆实时位置", current_time=None, blocking=True):
        """
        实时可视化车辆位置
        
        参数:
        vehicles_data: 车辆数据列表，每个元素是包含以下键的字典:
            - 'vehicle_id': 车辆ID
            - 'current_node': 当前节点
            - 'next_node': 下一个节点（如果在行驶中）
            - 'edge_progress': 在当前边的进度 (0-1)
            - 'status': 车辆状态
            - 'path': 完整路径
            - 'color': 车辆颜色
        title: 图表标题
        current_time: 当前时间
        blocking: 是否阻塞等待用户关闭窗口
        """
        if len(self.graph.nodes) == 0:
            print("没有可可视化的图数据")
            return None
        
        # 创建图形
        plt.figure(figsize=(16, 12))
        
        # 获取节点位置
        pos = {node: (data['longitude'], data['latitude']) 
               for node, data in self.graph.nodes(data=True)}
        
        # 绘制边 - 统一使用灰色
        for u, v, data in self.graph.edges(data=True):
            x1, y1 = pos[u]
            x2, y2 = pos[v]
            
            # 绘制道路线（统一灰色）
            plt.plot([x1, x2], [y1, y2], 'gray', alpha=0.5, linewidth=2, zorder=1)
        
        # 绘制节点 - 显示节点名称
        for node, (x, y) in pos.items():
            # 绘制节点
            plt.scatter(x, y, color='red', s=150, zorder=5, alpha=0.9, 
                       edgecolors='darkred', linewidth=2)
            
            # 显示节点名称
            plt.text(x, y, node, 
                    fontsize=10, ha='center', va='center', 
                    fontweight='bold', color='white', zorder=6)
        
        # 绘制车辆位置
        for vehicle_data in vehicles_data:
            vehicle_id = vehicle_data['vehicle_id']
            current_node = vehicle_data['current_node']
            next_node = vehicle_data.get('next_node')
            edge_progress = vehicle_data.get('edge_progress', 0)
            status = vehicle_data['status']
            path = vehicle_data.get('path', [])
            color = vehicle_data.get('color', 'blue')
            
            if status == "arrived":
                # 已到达的车辆，显示在终点
                if path:
                    end_node = path[-1]
                    x, y = pos[end_node]
                    plt.scatter(x, y, color=color, s=200, marker='s', zorder=10, 
                               edgecolors='black', linewidth=2, alpha=0.9)
                    plt.text(x, y+0.002, vehicle_id, fontsize=8, ha='center', 
                            fontweight='bold', color=color, zorder=11)
            
            elif status == "traveling" and next_node:
                # 行驶中的车辆，显示在边的当前位置
                x1, y1 = pos[current_node]
                x2, y2 = pos[next_node]
                
                # 计算车辆在边上的位置
                vehicle_x = x1 + (x2 - x1) * edge_progress
                vehicle_y = y1 + (y2 - y1) * edge_progress
                
                # 绘制车辆
                plt.scatter(vehicle_x, vehicle_y, color=color, s=200, marker='o', 
                           zorder=10, edgecolors='black', linewidth=2, alpha=0.9)
                plt.text(vehicle_x, vehicle_y+0.002, vehicle_id, fontsize=8, 
                        ha='center', fontweight='bold', color=color, zorder=11)
                
                # 绘制车辆轨迹（从起点到当前位置）
                if path:
                    path_index = path.index(current_node) if current_node in path else 0
                    for i in range(path_index):
                        u, v = path[i], path[i+1]
                        ux, uy = pos[u]
                        vx, vy = pos[v]
                        plt.plot([ux, vx], [uy, vy], color=color, alpha=0.6, 
                                linewidth=2, linestyle='--', zorder=2)
                    
                    # 当前边的轨迹
                    plt.plot([x1, vehicle_x], [y1, vehicle_y], color=color, 
                            alpha=0.8, linewidth=3, zorder=2)
            
            elif status == "waiting":
                # 等待中的车辆，显示在起点
                if path:
                    start_node = path[0]
                    x, y = pos[start_node]
                    plt.scatter(x, y, color=color, s=150, marker='^', zorder=10, 
                               edgecolors='black', linewidth=2, alpha=0.7)
                    plt.text(x, y+0.002, vehicle_id, fontsize=8, ha='center', 
                            fontweight='bold', color=color, zorder=11)
        
        # 设置地图显示范围
        all_lons = [pos[node][0] for node in self.graph.nodes()]
        all_lats = [pos[node][1] for node in self.graph.nodes()]
        
        lon_margin = (max(all_lons) - min(all_lons)) * 0.15
        lat_margin = (max(all_lats) - min(all_lats)) * 0.15
        
        plt.xlim(min(all_lons) - lon_margin, max(all_lons) + lon_margin)
        plt.ylim(min(all_lats) - lat_margin, max(all_lats) + lat_margin)
        
        # 设置标题
        if current_time:
            title_with_time = f"{title} - {current_time.strftime('%H:%M:%S')}"
        else:
            title_with_time = title
        
        plt.title(title_with_time, fontsize=18, fontweight='bold', pad=20)
        plt.xlabel('经度', fontsize=12)
        plt.ylabel('纬度', fontsize=12)
        plt.grid(True, alpha=0.3)
        
        # 添加图例
        legend_elements = []
        status_counts = {}
        for vehicle_data in vehicles_data:
            status = vehicle_data['status']
            color = vehicle_data.get('color', 'blue')
            status_key = f"{status}_{color}"
            if status_key not in status_counts:
                status_counts[status_key] = {'status': status, 'color': color, 'count': 0}
            status_counts[status_key]['count'] += 1
        
        for key, data in status_counts.items():
            status = data['status']
            color = data['color']
            count = data['count']
            status_text = {
                'traveling': '行驶中',
                'waiting': '等待中',
                'arrived': '已到达'
            }.get(status, status)
            
            if status == 'traveling':
                marker = 'o'
            elif status == 'waiting':
                marker = '^'
            else:
                marker = 's'
            
            from matplotlib.lines import Line2D
            legend_elements.append(Line2D([0], [0], marker=marker, color='w', 
                                         markerfacecolor=color, markersize=10, 
                                         label=f"{status_text} ({count}辆)"))
        
        if legend_elements:
            plt.legend(handles=legend_elements, loc='upper right')
        
        plt.tight_layout()
        
        # 显示图形
        if blocking:
            plt.show(block=True)  # 阻塞等待用户关闭窗口
        else:
            plt.show(block=False)
            plt.pause(0.1)  # 短暂暂停以更新图形
        
        return plt.gcf()
    
    def visualize_network_with_tables(self):
        """使用两个表格可视化路网（包含限速信息）"""
        if len(self.graph.nodes) == 0:
            print("没有可可视化的图数据")
            return
        
        # 创建图形 - 左侧是地图，右侧是两个表格
        fig = plt.figure(figsize=(30, 16))
        gs = plt.GridSpec(2, 2, width_ratios=[2, 1], height_ratios=[1, 1])
        
        ax_map = fig.add_subplot(gs[:, 0])  # 地图占据左侧全部
        ax_table1 = fig.add_subplot(gs[0, 1])  # 第一个表格：节点信息
        ax_table2 = fig.add_subplot(gs[1, 1])  # 第二个表格：路径信息（包含限速）
        
        # 获取节点位置
        pos = {node: (data['longitude'], data['latitude']) 
               for node, data in self.graph.nodes(data=True)}
        
        # 绘制边 - 统一使用蓝色，不标注距离
        for u, v, data in self.graph.edges(data=True):
            x1, y1 = pos[u]
            x2, y2 = pos[v]
            
            # 绘制道路线（统一蓝色）
            ax_map.plot([x1, x2], [y1, y2], 'b-', alpha=0.7, linewidth=2.5, zorder=1)
        
        # 绘制节点 - 只显示编号
        for node, (x, y) in pos.items():
            node_id = self.node_mapping[node]
            
            # 绘制节点
            ax_map.scatter(x, y, color='red', s=200, zorder=5, alpha=0.9, 
                          edgecolors='darkred', linewidth=2)
            
            # 只显示节点编号（大字体，清晰）
            ax_map.text(x, y, f"{node_id}", 
                       fontsize=14, ha='center', va='center', 
                       fontweight='bold', color='white', zorder=6)
        
        # 设置地图显示范围
        all_lons = [pos[node][0] for node in self.graph.nodes()]
        all_lats = [pos[node][1] for node in self.graph.nodes()]
        
        lon_margin = (max(all_lons) - min(all_lons)) * 0.15
        lat_margin = (max(all_lats) - min(all_lats)) * 0.15
        
        ax_map.set_xlim(min(all_lons) - lon_margin, max(all_lons) + lon_margin)
        ax_map.set_ylim(min(all_lats) - lat_margin, max(all_lats) + lat_margin)
        
        ax_map.set_title('西安市雁塔区主要道路网络 - 节点编号图', 
                        fontsize=18, fontweight='bold', pad=20)
        ax_map.set_xlabel('经度', fontsize=12)
        ax_map.set_ylabel('纬度', fontsize=12)
        ax_map.grid(True, alpha=0.3)
        
        # 第一个表格：节点信息表
        ax_table1.axis('off')
        ax_table1.set_title('节点信息表', fontsize=16, fontweight='bold', pad=20)
        
        # 准备节点表格数据
        node_table_data = []
        for node, data in self.graph.nodes(data=True):
            node_table_data.append([
                self.node_mapping[node],
                node,
                f"{data['longitude']:.4f}",
                f"{data['latitude']:.4f}"
            ])
        
        # 按节点编号排序
        node_table_data.sort(key=lambda x: x[0])
        
        # 创建节点表格
        node_table = ax_table1.table(
            cellText=node_table_data,
            colLabels=['编号', '节点名称', '经度', '纬度'],
            cellLoc='center',
            loc='center',
            bbox=[0.1, 0.1, 0.8, 0.8]
        )
        
        # 设置节点表格样式
        node_table.auto_set_font_size(False)
        node_table.set_fontsize(10)
        node_table.scale(1, 1.8)
        
        # 设置表头样式
        for i in range(4):
            node_table[(0, i)].set_facecolor('#4CAF50')
            node_table[(0, i)].set_text_props(weight='bold', color='white')
        
        # 设置交替行颜色
        for i in range(1, len(node_table_data) + 1):
            if i % 2 == 0:
                for j in range(4):
                    node_table[(i, j)].set_facecolor('#f0f0f0')
        
        # 第二个表格：路径信息表（包含限速） - 在表格旁边添加中文标注
        ax_table2.axis('off')
        ax_table2.set_title('路径信息表（包含限速和基准通行时间）', fontsize=16, fontweight='bold', pad=20)
        
        # 准备路径表格数据
        path_table_data = []
        for i, (u, v, data) in enumerate(self.graph.edges(data=True), 1):
            path_table_data.append([
                i,
                f"{self.node_mapping[u]}→{self.node_mapping[v]}",
                data['name'],
                f"{data['length']:.0f}",
                f"{data['speed_limit']}",
                f"{data['base_time']:.1f}",
                self.get_road_type_chinese(data['road_type'])
            ])
        
        # 创建路径表格
        path_table = ax_table2.table(
            cellText=path_table_data,
            colLabels=['序号', '节点路径', '道路名称', '长度(m)', '限速(km/h)', '基准时间(s)', '道路类型'],
            cellLoc='center',
            loc='center',
            bbox=[0.05, 0.1, 0.9, 0.8]
        )
        
        # 设置路径表格样式
        path_table.auto_set_font_size(False)
        path_table.set_fontsize(9)
        path_table.scale(1, 1.8)
        
        # 设置表头样式
        for i in range(7):
            path_table[(0, i)].set_facecolor('#2196F3')
            path_table[(0, i)].set_text_props(weight='bold', color='white')
        
        # 设置交替行颜色
        for i in range(1, len(path_table_data) + 1):
            if i % 2 == 0:
                for j in range(7):
                    path_table[(i, j)].set_facecolor('#e3f2fd')
        
        # 在表格旁边添加中文标注说明
        annotation_text = (
            "表格说明：\n"
            "1. 长度单位：米(m)\n"
            "2. 限速单位：千米/小时(km/h)\n"
            "3. 基准时间：在畅通状态下的通行时间，单位秒(s)\n"
            "4. 道路类型说明：\n"
            "   - 主干道：城市主要交通干道\n"
            "   - 次干道：连接主干道的道路\n"
            "   - 支路：连接次干道或直接服务区域的道路"
        )
        
        ax_table2.text(0.02, -0.15, annotation_text, 
                      transform=ax_table2.transAxes, fontsize=9,
                      verticalalignment='top',
                      bbox=dict(boxstyle="round,pad=0.3", facecolor='lightyellow', alpha=0.8))
        
        plt.tight_layout()
        plt.show()
        
        return pos
    
    def get_road_type_chinese(self, road_type):
        """将道路类型转换为中文"""
        type_mapping = {
            'motorway': '高速公路',
            'trunk': '主干道',
            'primary': '主干道',
            'secondary': '次干道',
            'tertiary': '支路',
            'residential': '居民区道路',
            'unclassified': '未分类道路'
        }
        
        if isinstance(road_type, list):
            road_type = road_type[0]
        return type_mapping.get(road_type, '未分类道路')
    
    def export_network_data(self, filename='yanta_road_network_data.txt'):
        """将网络数据导出到文本文件"""
        with open(filename, 'w', encoding='utf-8') as f:
            f.write("=" * 80 + "\n")
            f.write("西安市雁塔区道路网络数据\n")
            f.write("=" * 80 + "\n\n")
            
            # 1. 节点信息
            f.write("一、节点信息\n")
            f.write("-" * 70 + "\n")
            f.write(f"{'编号':<4} {'节点名称':<10} {'经度':<12} {'纬度':<12} {'连接道路数':<10}\n")
            f.write("-" * 70 + "\n")
            
            sorted_nodes = sorted(self.graph.nodes(data=True), 
                                key=lambda x: self.node_mapping[x[0]])
            
            for node, data in sorted_nodes:
                degree = self.graph.degree(node)
                f.write(f"{self.node_mapping[node]:<4} {node:<10} {data['longitude']:<12.4f} {data['latitude']:<12.4f} {degree:<10}\n")
            
            f.write("\n\n")
            
            # 2. 道路信息
            f.write("二、道路详细信息\n")
            f.write("-" * 90 + "\n")
            f.write(f"{'序号':<4} {'节点路径':<8} {'名称路径':<20} {'道路名称':<10} {'长度(m)':<10} {'限速(km/h)':<12} {'基准时间(s)':<15} {'道路类型':<10}\n")
            f.write("-" * 90 + "\n")
            
            total_length = 0
            total_base_time = 0
            total_speed = 0
            
            for i, (u, v, data) in enumerate(self.graph.edges(data=True), 1):
                length = data['length']
                speed_limit = data['speed_limit']
                base_time = data['base_time']
                road_type_ch = self.get_road_type_chinese(data['road_type'])
                
                total_length += length
                total_base_time += base_time
                total_speed += speed_limit
                
                node_path = f"{self.node_mapping[u]}→{self.node_mapping[v]}"
                name_path = f"{u}→{v}"
                
                f.write(f"{i:<4} {node_path:<8} {name_path:<20} {data['name']:<10} {length:<10.0f} {speed_limit:<12} {base_time:<15.1f} {road_type_ch:<10}\n")
            
            f.write("\n\n")
            
            # 3. 统计信息
            f.write("三、统计信息\n")
            f.write("-" * 50 + "\n")
            f.write(f"节点总数: {len(self.graph.nodes)}个\n")
            f.write(f"道路总数: {len(self.graph.edges)}条\n")
            f.write(f"总道路长度: {total_length:.1f} 米 ({total_length/1000:.2f} 公里)\n")
            f.write(f"总基准通行时间: {total_base_time:.1f} 秒 ({total_base_time/60:.1f} 分钟)\n")
            f.write(f"平均道路长度: {total_length/len(self.graph.edges):.1f} 米\n")
            f.write(f"平均基准通行时间: {total_base_time/len(self.graph.edges):.1f} 秒\n")
            f.write(f"平均限速: {total_speed/len(self.graph.edges):.1f} km/h\n")
            
            # 4. 限速分析
            f.write("\n四、限速分析\n")
            f.write("-" * 50 + "\n")
            
            # 按限速分组统计
            speed_groups = {}
            for _, _, data in self.graph.edges(data=True):
                speed = data['speed_limit']
                if speed not in speed_groups:
                    speed_groups[speed] = []
                speed_groups[speed].append(data['name'])
            
            for speed in sorted(speed_groups.keys(), reverse=True):
                roads = speed_groups[speed]
                f.write(f"{speed} km/h: {len(roads)}条道路\n")
                f.write(f"  道路列表: {', '.join(roads)}\n")
            
            # 5. 道路类型统计
            f.write("\n五、道路类型统计\n")
            f.write("-" * 50 + "\n")
            
            type_groups = {}
            for _, _, data in self.graph.edges(data=True):
                road_type = self.get_road_type_chinese(data['road_type'])
                if road_type not in type_groups:
                    type_groups[road_type] = 0
                type_groups[road_type] += 1
            
            for road_type, count in type_groups.items():
                f.write(f"{road_type}: {count}条\n")
        
        print(f"网络数据已导出到文件: {filename}")
    
    def print_detailed_network_info(self):
        """打印详细的网络信息（包含限速）"""
        print("\n" + "="*90)
        print("西安市雁塔区主要道路网络详细信息（包含限速）")
        print("="*90)
        
        print(f"\n节点信息 ({len(self.graph.nodes)}个):")
        print("-" * 70)
        print(f"{'编号':<4} {'节点名称':<10} {'经度':<12} {'纬度':<12} {'连接度':<8}")
        print("-" * 70)
        
        # 按编号排序输出
        sorted_nodes = sorted(self.graph.nodes(data=True), 
                            key=lambda x: self.node_mapping[x[0]])
        
        for node, data in sorted_nodes:
            degree = self.graph.degree(node)
            print(f"{self.node_mapping[node]:<4} {node:<10} {data['longitude']:<12.4f} {data['latitude']:<12.4f} {degree:<8}")
        
        print(f"\n路径详细信息 ({len(self.graph.edges)}条):")
        print("-" * 100)
        print(f"{'序号':<4} {'节点路径':<8} {'道路名称':<10} {'长度(米)':<10} {'限速(km/h)':<12} {'基准时间(秒)':<15} {'道路类型':<10}")
        print("-" * 100)
        
        total_length = 0
        total_base_time = 0
        total_speed = 0
        
        for i, (u, v, data) in enumerate(self.graph.edges(data=True), 1):
            length = data['length']
            speed_limit = data['speed_limit']
            base_time = data['base_time']
            road_type_ch = self.get_road_type_chinese(data['road_type'])
            
            total_length += length
            total_base_time += base_time
            total_speed += speed_limit
            
            node_path = f"{self.node_mapping[u]}→{self.node_mapping[v]}"
            
            print(f"{i:<4} {node_path:<8} {data['name']:<10} {length:<10.0f} {speed_limit:<12} {base_time:<15.1f} {road_type_ch:<10}")
        
        print(f"\n统计信息:")
        print(f"节点总数: {len(self.graph.nodes)}个")
        print(f"道路总数: {len(self.graph.edges)}条")
        print(f"总道路长度: {total_length:.1f} 米 ({total_length/1000:.2f} 公里)")
        print(f"总基准通行时间: {total_base_time:.1f} 秒 ({total_base_time/60:.1f} 分钟)")
        print(f"平均道路长度: {total_length/len(self.graph.edges):.1f} 米")
        print(f"平均基准通行时间: {total_base_time/len(self.graph.edges):.1f} 秒")
        print(f"平均限速: {total_speed/len(self.graph.edges):.1f} km/h")
        
        # 按限速分组显示
        print(f"\n限速分布:")
        speed_groups = {}
        for _, _, data in self.graph.edges(data=True):
            speed = data['speed_limit']
            if speed not in speed_groups:
                speed_groups[speed] = []
            speed_groups[speed].append(data['name'])
        
        for speed in sorted(speed_groups.keys(), reverse=True):
            roads = speed_groups[speed]
            print(f"  {speed} km/h: {len(roads)}条道路")
            if len(roads) <= 5:  # 如果道路数量不多，显示具体名称
                print(f"    道路: {', '.join(roads)}")

# 使用示例
def main():
    print("开始构建西安市雁塔区道路网络...")
    print("包含限速信息的版本")
    print("=" * 60)
    
    builder = AMapNetworkBuilderWithSpeed()
    
    # 创建路网
    network = builder.create_yanta_network_with_speed()
    
    if network:
        # 可视化路网（双表格版本，包含限速）
        positions = builder.visualize_network_with_tables()
        
        # 打印详细信息
        builder.print_detailed_network_info()
        
        # 导出数据到文件
        builder.export_network_data()
        
        print("\n路网构建和可视化完成！")
        print("说明：")
        print("1. 地图中蓝色线条表示道路，红色圆点表示节点")
        print("2. 左侧表格显示节点坐标信息")
        print("3. 右侧下方表格显示路径详细信息，包含限速和基准通行时间")
        print("4. 所有数据已导出到文件: yanta_road_network_data.txt")
        return network
    else:
        print("路网构建失败")
        return None

if __name__ == "__main__":
    network = main()
