def generate_data():
    f = open("datafile.txt", "w")
    for i in range(100000):
        f.write("1, 7.4, 9.3, meme, 6.66, 2, 0, 8.1, hatred, 12.3\n")
    f.close()



if __name__ == "__main__":
    generate_data()