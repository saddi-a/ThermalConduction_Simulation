import numpy as np
import matplotlib.pyplot as plt

# Define the file path
File = 'Resultat/Inhomogene.txt'

# Open the file
with open(File, 'r') as file:
    # Read the dimensions of the image
    size = list(map(int, file.readline().split()))

    # Initialize arrays
    result = np.zeros((size[1], size[0]))

    # Read espaceX
    espaceX = list(map(float, file.readline().split()))
    t_values = []

    # Read data and populate result
    for i in range(size[1]):
        tmp = list(map(float, file.readline().split()))
        t_values.append(tmp[0])  # Collect t values
        result[i, :] = np.array(tmp[1:]) - 273.15

# Plotting
plt.figure()
plt.imshow(result, extent=[min(espaceX), max(espaceX), max(t_values), min(t_values)], aspect='auto')
plt.title('Iron                 coper')
plt.xlabel('Distance in metre')
plt.ylabel('Time in seconde')
cb = plt.colorbar()
cb.set_label('Temperature in °C')
plt.show()