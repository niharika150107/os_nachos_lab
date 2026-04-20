// pipe.cc — kernel pipe implementation for Nachos
// Depends only on synch.h (Semaphore, Lock) — no system.h / main.h needed.

#include "pipe.h"
#include <string.h>
#include <stdio.h>
#include "synch.h"    // <-- here, not in pipe.h
#include "main.h"
#include "kernel.h"
// ============================================================
//  Pipe  —  one circular-buffer pipe with blocking read/write
// ============================================================

Pipe::Pipe(char *pipeName) {
    head   = 0;
    tail   = 0;
    count  = 0;
    isUsed = true;

    emptySlots = new Semaphore(pipeName, PIPE_BUF_SIZE);
    fullSlots  = new Semaphore(pipeName, 0);
    mutex      = new Lock(pipeName);
}

Pipe::~Pipe() {
    delete emptySlots;
    delete fullSlots;
    delete mutex;
}

int Pipe::Write(char *buf, int nBytes) {
    for (int i = 0; i < nBytes; i++) {
        emptySlots->P();
        mutex->Acquire();
        buffer[tail] = buf[i];
        tail = (tail + 1) % PIPE_BUF_SIZE;
        count++;
        mutex->Release();
        fullSlots->V();
    }
    return nBytes;
}

int Pipe::Read(char *buf, int nBytes) {
    for (int i = 0; i < nBytes; i++) {
        fullSlots->P();
        mutex->Acquire();
        buf[i] = buffer[head];
        head = (head + 1) % PIPE_BUF_SIZE;
        count--;
        mutex->Release();
        emptySlots->V();
    }
    return nBytes;
}

// ============================================================
//  PipeDescriptors  —  global table of all pipes
// ============================================================

PipeDescriptors::PipeDescriptors() {
    tableLock = new Lock("PipeDescriptors");
    for (int i = 0; i < MAX_PIPES; i++)
        pipes[i] = NULL;
    for (int i = 0; i < MAX_DES; i++) {
        des[i].pipe   = NULL;
        des[i].isRead = false;
    }
}

PipeDescriptors::~PipeDescriptors() {
    for (int i = 0; i < MAX_PIPES; i++)
        if (pipes[i]) delete pipes[i];
    delete tableLock;
}

int PipeDescriptors::allocDes() {
    for (int i = 0; i < MAX_DES; i++)
        if (des[i].pipe == NULL) return i;
    return -1;
}

int PipeDescriptors::allocPipe() {
    for (int i = 0; i < MAX_PIPES; i++)
        if (pipes[i] == NULL) return i;
    return -1;
}

int PipeDescriptors::createDes(int *readDes, int *writeDes, char *name) {
    tableLock->Acquire();

    int pipeIdx = allocPipe();
    if (pipeIdx < 0) {
        printf("PipeDescriptors: no free pipe slot\n");
        tableLock->Release();
        return -1;
    }

    int rdIdx = allocDes();
    if (rdIdx < 0) {
        printf("PipeDescriptors: no free descriptor for read-end\n");
        tableLock->Release();
        return -1;
    }

    des[rdIdx].pipe = (Pipe*)1;   // placeholder so allocDes skips it

    int wrIdx = allocDes();
    if (wrIdx < 0) {
        printf("PipeDescriptors: no free descriptor for write-end\n");
        des[rdIdx].pipe = NULL;
        tableLock->Release();
        return -1;
    }

    pipes[pipeIdx] = new Pipe(name);

    des[rdIdx].pipe   = pipes[pipeIdx];
    des[rdIdx].isRead = true;

    des[wrIdx].pipe   = pipes[pipeIdx];
    des[wrIdx].isRead = false;

    *readDes  = rdIdx;
    *writeDes = wrIdx;

    tableLock->Release();
    return 0;
}

int PipeDescriptors::readDes(int desNum, char *buffer, int nBytes) {
    if (desNum < 0 || desNum >= MAX_DES) return -1;
    if (des[desNum].pipe == NULL)        return -1;
    if (!des[desNum].isRead)             return -1;
    return des[desNum].pipe->Read(buffer, nBytes);
}

int PipeDescriptors::writeDes(int desNum, char *buffer, int nBytes) {
    if (desNum < 0 || desNum >= MAX_DES) return -1;
    if (des[desNum].pipe == NULL)        return -1;
    if (des[desNum].isRead)              return -1;
    return des[desNum].pipe->Write(buffer, nBytes);
}
