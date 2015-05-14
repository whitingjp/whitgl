#include <stdbool.h>
#include <stdio.h>

#include <whitgl/file.h>
#include <whitgl/logging.h>

bool whitgl_file_save(const char* fileName, int size, const void* data)
{
  FILE *dest;
  int written;
  dest = fopen(fileName, "wb");
  if (dest == NULL)
  {
    WHITGL_LOG("Failed to open %s for save.", fileName);
    return false;
  }

  written = fwrite(&size, 1, sizeof(size), dest);
  if(written != sizeof(size))
  {
    WHITGL_LOG("Failed to write size to %s", fileName);
    fclose(dest);
    return false;
  }

  written = fwrite(data, 1, size, dest);
  if(written != size)
  {
    WHITGL_LOG("Failed to write object to %s", fileName);
    fclose(dest);
    return false;
  }
  WHITGL_LOG("Saved data to %s", fileName);
  fclose(dest);
  return true;
}

bool whitgl_file_load(const char* fileName, int size, void* data)
{
  FILE *src;
  int read;
  int readSize;
  src = fopen(fileName, "rb");
  if (src == NULL)
  {
    WHITGL_LOG("Failed to open %s for load.", fileName);
    return false;
  }
  read = fread( &readSize, 1, sizeof(readSize), src );
  if(read != sizeof(readSize))
  {
    WHITGL_LOG("Failed to read size from %s", fileName);
    fclose(src);
    return false;
  }
  if(readSize != size)
  {
    WHITGL_LOG("Read size (%d) does not equal data structure size (%d)", readSize, size);
    return false;
  }
  read = fread( data, 1, size, src );
  if(read != size)
  {
    WHITGL_LOG("Failed to read object from %s", fileName);
    fclose(src);
    return false;
  }
  WHITGL_LOG("Loaded data from %s", fileName);
  fclose(src);
  return true;
}

bool whitgl_file_delete(const char* fileName)
{
  int result = remove(fileName);
  if(result != 0)
  {
    WHITGL_LOG("Failed to delete %s", fileName);
    return false;
  }
  return true;
}
