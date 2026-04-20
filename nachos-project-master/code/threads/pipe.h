#ifndef PIPE_H
#define PIPE_H

class Semaphore;
class Lock;
#define MAX_PIPES      10    // max number of pipes alive at once
#define PIPE_BUF_SIZE  256   // circular buffer size per pipe (bytes)

// ---------------------------------------------------------------------------
// Pipe -- one kernel pipe object
//   * circular byte buffer protected by a mutex
//   * two counting semaphores for producer/consumer synchronisation
// ---------------------------------------------------------------------------
class Pipe {
  public:
    Pipe(char *pipeName);
    ~Pipe();

    // Write nBytes from buf into the pipe (blocks if full)
    int Write(char *buf, int nBytes);

    // Read up to nBytes from the pipe into buf (blocks if empty)
    int Read(char *buf, int nBytes);

    bool isUsed;          // slot occupied?

  private:
    char  buffer[PIPE_BUF_SIZE];
    int   head;           // read  position
    int   tail;           // write position
    int   count;          // bytes currently in buffer

    Semaphore *emptySlots;  // counts free bytes  (init = PIPE_BUF_SIZE)
    Semaphore *fullSlots;   // counts used bytes  (init = 0)
    Lock      *mutex;       // mutual exclusion on head/tail/count
};

// ---------------------------------------------------------------------------
// PipeDescriptors -- the global table of all pipes
//   * descriptor numbers are small integers (like UNIX file descriptors)
//   * two descriptors are allocated per pipe: one read-end, one write-end
// ---------------------------------------------------------------------------
class PipeDescriptors {
  public:
    PipeDescriptors();
    ~PipeDescriptors();

    // Allocate a new pipe.  *readDes and *writeDes receive the descriptor
    // numbers for the read-end and write-end respectively.
    // Returns 0 on success, -1 on failure.
    int createDes(int *readDes, int *writeDes, char *name);

    // Read nBytes from the pipe identified by desNum into buffer.
    // Returns the number of bytes actually read, or -1 on error.
    int readDes(int desNum, char *buffer, int nBytes);

    // Write nBytes from buffer into the pipe identified by desNum.
    // Returns the number of bytes actually written, or -1 on error.
    int writeDes(int desNum, char *buffer, int nBytes);

  private:
    // Each descriptor entry records which Pipe object it belongs to and
    // whether it is the read-end or the write-end.
    struct DesEntry {
        Pipe *pipe;      // NULL if slot free
        bool  isRead;    // true => read-end, false => write-end
    };

    static const int MAX_DES = MAX_PIPES * 2;  // two descriptors per pipe
    DesEntry des[MAX_DES];

    Pipe *pipes[MAX_PIPES];

    Lock *tableLock;   // protects des[] and pipes[] during allocation

    int  allocDes();   // find a free descriptor slot, return index or -1
    int  allocPipe();  // find a free pipe    slot, return index or -1
};

#endif // PIPE_H
