/* ************************************************************************** */
/*                                                                            */
/*                                                   __                       */
/*   IPCMemory.h                                   <(o )___                   */
/*                                                  ( ._> /   - Weh.          */
/*   By: prp <tfm357@gmail.com>                    --`---'-------------       */
/*                                                 54 69 6E 66 6F 69 6C       */
/*   Created: 2017/09/15 18:28:30 by prp              2E 54 65 63 68          */
/*   Updated: 2017/09/25 17:07:22 by prp              50 2E 52 2E 50          */
/*                                                                            */
/* ************************************************************************** */

#ifndef _IPC_MEMORY_H
#define _IPC_MEMORY_H

#include <fcntl.h>
#include <iostream>
#include <mutex>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define HEADER_VALUE 0x626F726B // Bork Bork

namespace extension {
template <size_t block_size> class SharedMemoryObject {
  class SharedBlock {
  public:
    uint32_t header = 0;
    int status_flag = 0;
    pthread_mutex_t lock;
    uint8_t data[block_size];
  };

protected:
  int segment_id = -1;
  void *segment_addr = nullptr;
  bool segment_owner = false;
  std::string segment_name;

  SharedMemoryObject<block_size>::SharedBlock *block_ptr = nullptr;

  int get_status_flag() {
    lock();
    auto result = this->block_ptr->status_flag;
    unlock();
    return result;
  }

public:
  void init(const std::string &segment_name) {

    std::cout << "Initializing...";

    this->segment_id =
        shm_open(segment_name.c_str(), O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);

    this->segment_name = segment_name;

    if (segment_id < 0) {
      std::cerr << "Failed to open shared memory file descriptor.\n";
      return;
    };

    auto result = ftruncate(this->segment_id, block_size + sizeof(int));

    if (result != 0) {
      std::cerr << "Failed to resize shared memory, may be out of memory.\n";
      return;
    }

    std::cout << "\nMapping memory...\n";

    this->segment_addr =
        mmap(nullptr, block_size + sizeof(int), PROT_READ | PROT_WRITE,
             MAP_SHARED, this->segment_id, 0);

    if (this->segment_addr == (void *)-1) {
      this->segment_addr = nullptr;
      std::cerr << "Failed to map shared memory.\n";
      return;
    }

    this->block_ptr =
        (SharedMemoryObject<block_size>::SharedBlock *)segment_addr;

    if (this->block_ptr->header != HEADER_VALUE) {
      this->block_ptr->header = HEADER_VALUE;
      this->block_ptr->status_flag = 0;
      pthread_mutexattr_t mutex_attr;
      pthread_mutexattr_init(&mutex_attr);
      pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
      pthread_mutex_init(&block_ptr->lock, &mutex_attr);
      bzero(block_ptr->data, block_size);
    }

    this->lock();
    this->block_ptr->status_flag += 1;
    this->unlock();
  }

  SharedMemoryObject<block_size>() {}

  ~SharedMemoryObject<block_size>() {
    if (this->segment_id > 0)
      close(this->segment_id);

    if (this->segment_addr)
      munmap(this->segment_addr, block_size + sizeof(int));

    if (this->get_status_flag() <= 1) {
      pthread_mutex_unlock(&this->block_ptr->lock);
      bzero(&this->block_ptr->header, sizeof(uint32_t));
      shm_unlink(this->segment_name.c_str());
    } else {
      this->lock();
      this->block_ptr->status_flag -= 1;
      this->unlock();
    }
  }

  void lock() { pthread_mutex_lock(&this->block_ptr->lock); }

  void unlock() { pthread_mutex_unlock(&this->block_ptr->lock); }

  uint8_t *get_data_ptr() {
    if (!this->block_ptr)
      return nullptr;

    return block_ptr->data;
  }
};
}

#endif
