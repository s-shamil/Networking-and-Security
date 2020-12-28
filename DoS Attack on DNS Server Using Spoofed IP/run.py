import os, time
for i in range(10):
	start_time = time.time()
	os.system('python query.py')

	print("--- %s seconds ---" % (time.time() - start_time))