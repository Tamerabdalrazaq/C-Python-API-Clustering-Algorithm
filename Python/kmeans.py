import sys
import math


EPSILON = 0.001
ITER = 200
error_messages = {
    "K": "Invalid number of clusters!",
    "N": "Invalid number of points!",
    "d": "Invalid dimension of point!",
    "iter": "Invalid maximum iteration!",
}
def main():
    K=N=d=iter= data = None
    args = sys.argv
    K= (args[1])
    N= (args[2])
    d= (args[3])
    
    
    if  len(sys.argv) >= 6:
        iter = args[4]
        if (not iter.isdigit()): return (print(error_messages["iter"]))
        else: iter = int(iter)

        data= readXconv(args[5])
    else:
        iter = ITER
        data= readXconv(args[4])

    #checks if the arguments are natural numbers
    if (not d.isdigit()): return (print(error_messages["d"]))
    if (not K.isdigit()): return (print(error_messages["K"]))
    if (not N.isdigit()): return (print(error_messages["N"]))
    
    K= int(K)
    N= int(N)
    d= int(d)

    #checks the validity of the arguments
    if (not N>1): return print(error_messages["N"])
    if (K<=1 or N<=K): return print(error_messages["K"])
    if(d<1): return print(error_messages["d"])
    if (not 1<iter<1000): return print(error_messages["iter"])

    output = k_means(K,N,d,iter,data)
    for row in output:
        print(",".join(map(str, row)))
    print("")
    


#reads the input file and converts its data to a matrix of vectors
def readXconv(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
        fl_strings = [line.split(',') for line in lines]
        fl_num = []
        for line in fl_strings:
            fl_num.append([float(str) for str in line])
    return fl_num

def k_means(k,n,d,iter,data):
    centroids = [(vector) for vector in data[0:k]]
    for i in range(0, iter):
        new_centroids = [[[0] * d, 0] for centroid in centroids] 
        for x in data:
            closest_centroid_index = find_closest_centroid_index(centroids, x)
            add_vector_to_centroid(new_centroids[closest_centroid_index], x)
        new_centroids =  [calc_centroid_average(cent) for cent in new_centroids]
        if check_centroid_convergence(centroids, new_centroids):
            centroids = new_centroids
            break
        centroids = new_centroids
        
    return  [[round(num, 4) for num in centroid] for centroid in centroids]

#finds the closest cluster of a vector
def find_closest_centroid_index(centroids, v):
    closest_centroid_tuple = (None, math.inf)
    for i  in range(len(centroids)):
        centroid = centroids[i]
        distance = euc_l2(centroid, v)
        if (distance < closest_centroid_tuple[1]):
            closest_centroid_tuple = (i, distance)
    return closest_centroid_tuple[0]

#calculates the average of all vectors in a cluster
def calc_centroid_average(centroid_size_tuple):
    centroid = centroid_size_tuple[0]
    size = centroid_size_tuple[1]
    if(size == 0):
        return [0]*len(centroid)
    return [x/size for x in centroid]

#computes the euclidean distance of two given vectors
def euc_l2(v1, v2):
    dist = 0
    for i in range(len(v1)):
        dist += math.pow(v1[i]-v2[i], 2)
    return math.sqrt(dist)

#adds a vector to a cluster
def add_vector_to_centroid(centroid_tuple, v):
    for i in range(len(centroid_tuple[0])):
        updated_entry_i = centroid_tuple[0][i] + v[i]
        centroid_tuple[0][i] = updated_entry_i
    centroid_tuple[1] += 1


#checks if the gap between two centroids is smaller than epsilon
def check_centroid_convergence(centroids, new_centroids):
    convergent_centroids = 0
    for i in range(len(centroids)):
        if euc_l2(centroids[i], new_centroids[i]) <= EPSILON:
            convergent_centroids += 1
    return convergent_centroids == len(centroids)

#in case there are errors in the runtime
# try:
#     main()
# except:
#     print("An Error Has Occured, kmeans")