
void RS485Serial1Begin();


int SWrite(char* buf,int size,long unsigned int* bytesWritten);
int SRead(char* buf,int size,long unsigned int* bytesRead);
int SAwaitEOF();
