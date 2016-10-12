unsigned char *readPGM(const char *name, int &w, int &h, bool adjustToEightMultiple = false);
bool writePGM(const char *name, int w, int h, unsigned char *data);
bool writePGM(const char *name, int w, int h, int *data);
bool writePGM(const char *name, int w, int h, double *data);
bool writePGM(const char *name, int w, int h, double *data, unsigned char *add_image);
bool writePGM(const char *name, int w, int h, float *data);
int getMagicNumber(int n);

