#include "globals.h"

QTcpServer *tcpServer = nullptr;
QTcpSocket *tcpSocket = nullptr;

const qint32 columns = 8;

const qint32 dir4X[4] = {0, 0, 1, -1};
const qint32 dir4Y[4] = {-1, 1, 0, 0};
const qint32 dir8X[8] = {0, 0, 1, -1, -1, -1, 1, 1};
const qint32 dir8Y[8] = {-1, 1, 0, 0, -1, 1, -1, 1};
const qint32 dirKnightX[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
const qint32 dirKnightY[8] = {1, 2, 2, 1, -1, -2, -2, -1};
