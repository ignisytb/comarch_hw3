from math import log
import matplotlib.pyplot as plt

def diff_y(x,y):
    return y*(log(x)-log(y))/x

x_final = []
y_final = []
intervals = [0.5, 0.25, 0.2, 0.1, 0.05, 0.025, 0.01, 0.001, 0.0001]

for i in intervals:
    x_interval = i
    target=2
    x = 1
    y = 4
    x_data = [x]
    y_data = [y]
    tf = target > x

    while(tf == (target>x)):
        if target>x:
            y += diff_y(x,y)*x_interval
            x += x_interval
            x_data.append(x)
            y_data.append(y)
        else:
            y -= diff_y(x,y)*x_interval
            x -= x_interval
            x_data = [x] + x_data
            y_data = [y] + y_data

    for idx, i in enumerate(x_data):
        if abs(i-target)<(x_interval*0.5):
            x_final.append(x_data[idx])
            y_final.append(y_data[idx])
            x_data = x_data[:idx+1]
            y_data = y_data[:idx+1]
            break

    plt.plot(x_data, y_data, linewidth=0.5, color="blue")

print(x_final)
print(y_final)

plt.title("Euler Method")
plt.xlabel("x")
plt.ylabel("y")
plt.show()

for idx, i in enumerate(x_final):
    print(str(intervals[idx])+" & "+str(y_final[idx])+" \\\\\\hline")