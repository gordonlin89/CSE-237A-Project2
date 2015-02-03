from __future__ import print_function
import sys
import csv

with open(sys.argv[1]) as f:
	data = [row for row in csv.DictReader(f)]

timesteps = [float(d['timestep']) for d in data]
time_diffs = [cur - prev for prev, cur in zip(timesteps[:-1], timesteps[1:])]
total_time = timesteps[-1]

cpu_energy = sum([float(row['cpu_power']) * time for row, time in zip(data[1:], time_diffs)])
gpu_energy = sum([float(row['gpu_power']) * time for row, time in zip(data[1:], time_diffs)])

cpu_edp = cpu_energy*total_time
gpu_edp = gpu_energy*total_time

cpu_power = cpu_energy / total_time
gpu_power = gpu_energy / total_time

print(cpu_power, cpu_energy, total_time, cpu_edp, sep=',')
print(gpu_power, gpu_energy, total_time, gpu_edp, sep=',')
print(cpu_power + gpu_power, cpu_energy + gpu_energy, total_time, cpu_edp + gpu_edp, sep=',')

