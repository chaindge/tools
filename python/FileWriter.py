class FileWriter:
    def __init__(self):
        self.file = None

    def __init__(self, file_name, mode):
        self.file = open(file_name, mode)

    def __del__(self):
        self.fclose()

    def fopen(self, file_name, mode):
        self.fclose()
        self.file = open(file_name, mode)

    def fclose(self):
        if self.file is not None:
            self.file.close()

    def fprint(self, content):
        if self.file is not None:
            self.file.write(content + '\n')

    def fwrite(self, content):
        if self.file is not None:
            self.file.write(content)
