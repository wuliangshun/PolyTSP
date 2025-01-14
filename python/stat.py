import re
import csv
from matplotlib.ticker import MultipleLocator
import time
import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
from matplotlib.ticker import MultipleLocator

plt.switch_backend('agg')

class Plot:
	
	@staticmethod
	def hist(data_series, colors, labels, bins_num, bins_split_rule, bar_type, remove_ticks, output_dist_png):
		'''
		@Paras: 
				data_series:         input data_series
				colors:              colors for each data series
				labels:              labels for each data series
				bins_num:            number of bins
				bins_split_rule:     'avg': interval = (maxx-minx)/bins_num
									 'exp': interval = minx*base^i   where  base=log_base(maxx/minx)
				bar_type:            'panels': 2,3,4 panels
				                     'bars'  : 2,3,4 bars in one panel
		'''
		
		# get maxx and minx
		minx = 10000000000000000000000000000000000000000000000000
		for series in data_series:
			if minx > min(series):
				minx = min(series)
		maxx = 0
		for series in data_series:
			if maxx < max(series):
				maxx = max(series)
		maxx = minx * 1.5
		print("max number:{}, min number:{}".format(maxx, minx))
		# compute interval
		interval = 0
		bins = []
		if bins_split_rule == 'avg':
			interval = int((maxx - minx)/bins_num)
			bins = [minx + interval*i for i in range(bins_num)]
		elif bins_split_rule == 'exp':
			pass
		print("bins:{}".format(bins))
		# bar setting
		bar_width = 0.2
		bar = list(range(len(bins)))
		if bar_type == 'panels':
			bar_x = [i-0.5 for i in bar] # xticks 
		elif bar_type == 'bars':
			if len(data_series) == 3:
				bar_1 = bar
				bar_2 = [i + bar_width for i in bar_1]
				bar_3 = [i + bar_width for i in bar_2]
			elif len(data_series) == 4:
				bar_1 = bar
				bar_2 = [i + bar_width for i in bar_1]
				bar_3 = [i + bar_width for i in bar_2]
				bar_4 = [i + bar_width for i in bar_3]
		
		# figure setting
		fig = plt.figure(figsize=(24,14))  #
		# plot
		for i in range(len(data_series)):
			series = data_series[i]
			count = [0] * bins_num
			for num in series:
				for j in range(len(bins) - 1):
					if num >= bins[j] and num <= bins[j+1]:
						count[j] += 1
			if bar_type == 'panels':
				ax = fig.add_subplot(len(data_series), 1, i + 1)
				if colors is None and labels is None:
					plt.bar(bar, count, width=bar_width)
				elif colors is None and labels is not None:
					plt.bar(bar, count, width=bar_width, label=labels[i])  
				elif colors is not None and labels is None:
					plt.bar(bar, count, width=bar_width, color=colors[i])
				else:
					plt.bar(bar, count, width=bar_width, color=colors[i], label=labels[i])  
				if i != len(data_series)-1:
					xlabels = ["" for i in range(len(bins))]  
				else:
					xlabels = []
					for i in range(len(bins)):
						s0 = str(format(bins[i], '.1e'))
						b0 = s0.split('e+')[0]
						e0 = s0.split('e+')[1][1]
						xlabels.append(b0 + "*$\mathregular{10^{" + e0 + "}}$") 
					if bins[-1] < 1000000:
						xlabels = bins
					#plt.tight_layout(h_pad=5.0,pad=18)  #h_pad=20.0,pad=10
				plt.xticks(bar_x, xlabels, rotation=90) # fontsize=8
			if remove_ticks:
				ax.tick_params(bottom=False,top=False,left=False,right=False)  #移除全部刻度线
			#plt.legend()
			#plt.ylim(0,y_max)
		#plt.show()
		plt.savefig(output_dist_png)

class Draw(object):
    bound_x = []
    bound_y = []

    def __init__(self,mapxy,mapx,mapy):
        self.plt = plt
        self.fig, self.ax = self.plt.subplots()
        self.plt.figure(figsize=(100, 100))
        #self.set_font()
        self.bound_x = [min(mapx),max(mapx)]
        self.bound_y = [min(mapy),max(mapy)]
        self.set_xybound(self.bound_x,self.bound_y)
        if max(mapx) > 1000:
            x_major_locator=MultipleLocator(200)
            y_major_locator=MultipleLocator(200)
        else:
            x_major_locator=MultipleLocator(5)
            y_major_locator=MultipleLocator(5)
        self.ax.xaxis.set_major_locator(x_major_locator)
        self.ax.yaxis.set_major_locator(y_major_locator)
    
        
        
    def draw_line(self, p_from, p_to):
        line1 = [(p_from[0], p_from[1]), (p_to[0], p_to[1])]
        (line1_xs, line1_ys) = zip(*line1)
        self.ax.add_line(Line2D(line1_xs, line1_ys, linewidth=1, color='blue'))

    def draw_arrow(self, p_from, p_to):
        self.ax.arrow(p_from[0], p_from[1], p_to[0] - p_from[0], p_to[1] - p_from[1],
                      width = 0.1,length_includes_head=True,
                        head_width=(self.bound_x[1] - self.bound_x[0]) /100 ,
                        head_length=(self.bound_x[1] - self.bound_x[0]) /100,
                        fc='blue', ec='black')

    def draw_points(self, pointx, pointy):
        self.ax.scatter(pointx, pointy, s=3,c='r')

    def set_xybound(self, x_bd, y_bd):
        self.ax.axis([x_bd[0], x_bd[1], y_bd[0], y_bd[1]])

    def draw_text(self, x, y, text, size=8):
        self.ax.text(x, y, text, fontsize=size)

    def set_font(self, ft_style='SimHei'):
        plt.rcParams['font.sans-serif'] = [ft_style]  # 用来正常显示中文标签
        
    def save(self,name):
        self.fig.savefig(name, dpi=300)
		
class ParseLog:
	
	
	@staticmethod
	def merge_files(src_filepaths, output_filepath):
		f = open(output_filepath, "w")   
		for doc in src_filepaths:
			x = open (doc, "r")  
			while True:
				data = x.read(1024)
				if data:
					f.write(data)
				else:
					print('{} read over.'.format(doc))
					break  
			x.close() 
		f.close()  
	
	
	@staticmethod
	def parse_log_graph(file_path):
		"""
		根据日志，解析图结构（节点之间的距离）
		"""
		weights = []
		with open(file_path, 'r') as f:
			while True:
				line = f.readline()
				if not line:
					break
				reg = re.findall( '\d{0,2} linked to vertex \d{0,2} with weight (\f{0,2})',line)
				if len(reg) > 0:
					weights.append(int(reg[0]))
		weights.sort()
		print(weights)
		print(sum(weights[0:100]))
		
	@staticmethod
	def parse_tsp(tsp_path):
		mapxy = []
		with open(tsp_path)as f:
			rows = f.readlines()
			for row in rows:
				row_data = row.split(' ')
				print(row_data)
				if len(row_data) == 4:
					try:
						if int(row_data[1]) >= 0:
							x = float(row_data[2])
							y = float(row_data[3])
							mapxy.append([x,y])
					except:
						pass
		return mapxy
	
	@staticmethod
	def parse_stat_data(file_path):
		"""
		从日志中解析出每轮迭代的路径的距离
		"""
		temp_data_series = [[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[],[]]
		print(temp_data_series)
		with open(file_path, 'r') as f:
			while True:
				line = f.readline()
				if not line:
					break
				#if "BFS:" in line:
				#	line = line.replace("\n","")
				#	data = float(line.split(':')[-1])
				#	temp_data_series[0].append(data)
				#if "Shot Gun +1:" in line:
					#line = line.replace("\n","")
					#data = float(line.split(':')[-1])
					#temp_data_series[1].append(data)
				for i in range(len(temp_data_series)):
					if "Shot Gun & Fuzzy Concatenation +{}:".format(str(i+1)) in line:
						line = line.replace("\n","")
						data = float(line.split(':')[-1])
						if data==4:
							print(line)
						temp_data_series[i+2].append(data)
				
					
		data_series = []
		for series in temp_data_series:
			if len(series) > 0:
				data_series.append(series)
				print('series:min:{},max:{}'.format(min(series),max(series)))		
		print('data prepared, {} series'.format(len(data_series)))
		return data_series
	
	@staticmethod
	def parse_log_path(file_path):
		"""
		从日志中找出所有的路径和对应的距离
		"""
		paths = []
		costs = []
		with open(file_path, 'r') as f:
			while True:
				line = f.readline()
				if not line:
					break
				line_datas = re.findall("(\d{1,5})   *| Cost:(\d{1,10}.\d{0,10})\n", line)
				if(len(line_datas) > 5):
					path = []
					cost = 0
					for pair in line_datas:
						if pair[0] != '':
							path.append(int(pair[0]))
						if pair[1] != '':
							cost = float(pair[1])
					if len(path) > 0 and cost != 0:
						paths.append(path)
						costs.append(cost)
						#print(path,cost)
		return paths,costs
					
					
					

class Test:
	
	@staticmethod
	def draw_path(tsp_path, output_path, path):
		"""
		绘制路径图
		"""
		mapxy = ParseLog.parse_tsp(tsp_path)
		print(mapxy)
		#直接传入path
		path_map = path# [38, 39, 37, 36, 47, 23, 14, 4, 5, 3, 24, 11, 27, 26, 25, 46, 12, 13, 51, 10, 50, 32, 42, 9, 8, 7, 40, 18, 44, 31, 48, 0, 21, 49, 19, 22, 20, 30, 17, 2, 16, 6, 1, 41, 29, 28, 15, 45, 43, 33, 34, 35, 38]
		mapxy_final = []
		mapx_final = []
		mapy_final = []
		for path in path_map:
			mapxy_final.append(mapxy[path])
			mapx_final.append(mapxy[path][0])
			mapy_final.append(mapxy[path][0])
		print(mapxy_final)
		dw = Draw(mapxy_final,mapx_final,mapy_final)
		#标出点
		for map in mapxy_final:
			dw.draw_points(map[0],map[1])
		#标出序号
		for i in range(len(mapxy_final)):
			dw.draw_text(mapxy_final[i][0],mapxy_final[i][1],str(i))

        #画箭头
		for j in range(len(mapxy_final)-1):
			pltx = dw.draw_arrow(mapxy_final[j],mapxy_final[j+1])
		dw.save(output_path)
	
	@staticmethod
	def find_shortest_path(log_path):
		"""
		找到最短路径
		"""
		paths,costs = ParseLog.parse_log_path(log_path)
		inxs = sorted(range(len(costs)), key=lambda k: costs[k])
		print("shorest path: {}, cost: {}".format(paths[inxs[0]], costs[inxs[0]]))
		return paths[inxs[0]]
	
	@staticmethod
	def stat_all_methods(log_path, output_dist_png):
		"""
		绘制统计分布图
		"""
		data_series = ParseLog.parse_stat_data(log_path)
		#labels = ['Gun Shot']
		#for i in range(100):
			#labels.append('Gun Shot & Fuzzy Concatenation*'+str(i+1))
		Plot.hist(data_series = data_series,
				  colors = None, 
				  labels = None, 
				  bins_num = 40, 
				  bins_split_rule = "avg", 
				  bar_type = "panels",
				  remove_ticks = False,
				  output_dist_png = output_dist_png)

if __name__ == '__main__':
	data_path= "../data/berlin52.tsp"
	log_path = "../src/Logs/2020_11_9.txt"
	output_path_png = "path_2020_11_9.png"
	output_dist_png = "dist_2020_11_9.png"
	#ParseLog.parse_graph(log_path)
	#ParseLog.merge_files(["../src/Logs/2020_10_31.txt","../src/Logs/2020_11_1.txt"], "../src/Logs/eil76_60_50000_10000_200.txt")
	
	#shortest_path = Test.find_shortest_path(log_path)
	#shortest_path = [22,21,34,35,36,37,38,51,50,49,48,47,46,45,44,58,59,60,61,62,63,64,77,76,75,74,73,72,71,84,85,86,87,88,89,90,103,102,101,99,100,114,115,116,129,128,127,141,142,154,155,167,168,180,181,194,193,192,191,190,189,188,187,186,185,184,183,182,169,170,171,172,173,174,175,176,177,178,179,166,165,164,163,162,161,160,159,158,157,156,143,144,145,146,147,148,149,150,151,152,153,140,139,138,137,136,135,134,133,132,131,130,117,118,119,120,121,122,123,124,125,126,113,112,111,98,97,110,109,108,107,106,105,104,91,92,93,94,95,96,83,82,81,80,79,78,65,66,67,68,69,70,57,56,55,54,53,52,39,40,41,42,43,28,29,30,31,32,33,20,19,18,17,16,15,14,27,26,13,0,1,2,3,4,5,6,7,8,9,10,11,12,25,24,23,22]
	#Test.draw_path(data_path, output_path_png, shortest_path)
	Test.stat_all_methods(log_path, output_dist_png)
