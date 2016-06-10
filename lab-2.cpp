import os
import itertools

class BankerAlgo():
	def __init__(self):
		self.get_inputs()
		self.process()

	def get_inputs(self):
		self.n_process = int(raw_input('P1 of processes:'))
		self.n_resource = int(raw_input('P2 of resources:'))

		self.resources = [int(each) for each in raw_input('total of each resource:').split(' ')]

		self.demands = []
		self.allocations = []

		for idx in range(self.n_process):
			n = idx + 1
			demand = [int(each) for each in raw_input('P3%d process\'s max demands to every resouce:' % n).split(' ')]
			allocation = [int(each) for each in raw_input('P4%d process\'s allocation already to every resouce:' % n).split(' ')]

			if len(demand) != len(allocation) or len(demand) != self.n_resource:
				print "data error, exit program!"
				os._exit()

			self.demands.append(demand)
			self.allocations.append(allocation)

		if len(self.demands) != self.n_process:
			print "data error, exit program!"
			
	def process(self):
		for each in itertools.permutations([x for x in range(self.n_process)]):
			if self.verify_path(each):
				print each
				return ;

		print "false"

	def verify_path(self, path):
		# get the temp variables
		lefts = []
		for idx in range(self.n_resource):
			lefts.append(self.resources[idx] - sum([self.allocations[idx_proc][idx] for idx_proc in range(self.n_process)]))

		demands_lefts = []
		for idx in range(self.n_process):
			demands_lefts.append([self.demands[idx][each_res] - self.allocations[idx][each_res] for each_res in range(self.n_resource)])

		# verify the results
		for current in path:
			cur_process_demand = demands_lefts[current]
			for idx in range(self.n_resource):
				if lefts[idx] < cur_process_demand[idx]:
					return False

			for idx in range(self.n_resource):
				lefts[idx] += self.allocations[current][idx]

		return True

if __name__ == '__main__':
	bank = BankerAlgo()
	input()
