import sys
import numpy as np
import pandas as pd
from sklearn.metrics import silhouette_score
import kmeans
import symnmf
ITER = 200

#error func with error message 
def error():
    print("An Error Has Occurred")
    exit()


#reads the file from command line
def read(data):
    df = pd.read_csv(data, header=None)
    np_array = df.to_numpy()
    return np_array


#calculates the centroids using kmeans, outputs the indices of the vectors' clusters respectively 
def symnmf_clustering(matrix, N, d, k):
    D_matrix = matrix.flatten().tolist()
    symnmf_H = np.array(symnmf.symnmf(D_matrix, N, d, k))
    symnmf_H = symnmf_H.reshape((N, k))    
    indices = symnmf_H.argmax(axis=1)
    return indices


#outputs the indices of the vectors' clusters respectively using symnmf
def kmeans_clustering(matrix, N, k, d):
    centroids = kmeans.k_means(k, N, d, ITER, matrix)   
    indices = []
    for datapoint in matrix:
        indices.append(kmeans.find_closest_centroid_index(centroids, datapoint))
    return indices


def main():
    #argument validation
    args = sys.argv
    if (len(args) != 3):
        error()
    k = (args[1])
    file_name = (args[2])
    try:
        k = int(k)
        file_name = str(file_name)
    except:
        error()

    matrix = read(file_name)

    #dimensions
    N = matrix.shape[0]
    d = matrix.shape[1]

    #final indices 
    kmeans_clusters = kmeans_clustering(matrix, N, k ,d)  # calculating the centroids using kmeans
    symnmf_clusters = symnmf_clustering(matrix, N ,d, k)  # calculating the centroids using symnmf

    #calculates the silhouette_score using sklearn.metrics 
    print("nmf: %.4f" % silhouette_score(matrix, symnmf_clusters))
    print("kmeans: %.4f" % silhouette_score(matrix, kmeans_clusters))

main()
