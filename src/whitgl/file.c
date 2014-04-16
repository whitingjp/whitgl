#include <stdbool.h>
#include <stdio.h>

#include <jpw/file.h>
#include <jpw/logging.h>

bool jpw_file_save(const char* fileName, int size, const void* data)
{
  FILE *dest;
  int written;
  dest = fopen(fileName, "wb");
  if (dest == NULL)
  {
    JPW_LOG("Failed to open %s for save.", fileName);
    return false;
  }
    
  written = fwrite(&size, 1, sizeof(size), dest);
  if(written != sizeof(size))
  {
    JPW_LOG("Failed to write size to %s", fileName);
    fclose(dest);
    return false;
  }
  
  written = fwrite(data, 1, size, dest);
  if(written != size)
  {
    JPW_LOG("Failed to write object to %s", fileName);
    fclose(dest);
    return false;
  }
  JPW_LOG("Saved data to %s", fileName);
  fclose(dest);
  return true;
}

bool jpw_file_load(const char* fileName, int size, void* data)
{
  FILE *src;
  int read;
  int readSize;
  src = fopen(fileName, "rb");
  if (src == NULL)
  {
    JPW_LOG("Failed to open %s for load.", fileName);
    return false;
  }
  read = fread( &readSize, 1, sizeof(readSize), src );
  if(read != sizeof(readSize))
  {
    JPW_LOG("Failed to read size from %s", fileName);
    fclose(src);
    return false;
  }
  if(readSize != size)
  {
    JPW_LOG("Read size (%d) does not equal data structure size (%d)", readSize, size);
  }
  read = fread( data, 1, size, src );
  if(read != size)
  {
    JPW_LOG("Failed to read object from %s", fileName);
    fclose(src);
    return false;
  }
  JPW_LOG("Loaded data from %s", fileName);
  fclose(src);
  return true;
}

bool jpw_file_delete(const char* fileName)
{
  int result = remove(fileName);
  if(result != 0)
  {
    JPW_LOG("Failed to delete %s", fileName);
    return false;
  }
  return true;
}
