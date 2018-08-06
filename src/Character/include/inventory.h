#include <vector>

struct ItemData
{
  int id;
  int metadata;
};
class Inventory
{
  private:
    ItemData* itemArray;
    int invSize;
  public:

    Inventory(int InvSize = 20)
    {
      invSize = InvSize;
      ItemData = new ItemData[invSize];
      memset(itemArray,0,sizeof(ItemData)*invSize);
    }
    ~Inventory()
    {
      delete[] itemArray;
    }
    int getSize(){return invSize;}

    void resizeInventory(int newSize)
    {
      ItemData* newArr = new ItemData[newSize];
      memset(itemArray,0,sizeof(ItemData)*newSize);
      for(int i = 0;i<invSize;i++)
      {
        newArr[i] = itemArray[i];
      }

      delete[] itemArray;
      itemArray = newArr;
      invSize = newSize;
    }

    ItemData& getItem(int invPos)
    {
      if(invPos >= invSize)
      {
        return itemArray[invPos];
      }
    }

};
