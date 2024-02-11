import numpy as np
import matplotlib.pyplot as plt

fichier = "TypesDeMateriaux.txt"

with open(fichier, 'r') as fileID:
    taille = list(map(int, fileID.readline().split()))
    X, Y = taille[0], taille[1]

    t = 0
    affichage = np.zeros((Y, X))

    for i in range(Y):
        affichage[i, :] = list(map(float, fileID.readline().split()))

plt.figure()
plt.imshow(affichage, aspect='auto')
plt.title('Materiaux')
plt.savefig("Results/Materiaux.svg")
