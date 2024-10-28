    typedef struct _SuperBlock
    {
        bool inode_list_bit[128];
        bool data_block_bit[256];
    } SuperBlock;
    typedef struct _InodeList
    {
        bool type; 
        Date date;
        int size;   
    } InodeList;
    typedef struct _Date
    {
            int year;
            int month;
            int day;
            int hour;
            int minute;
            int second;
    } Date;
    
    typedef struct _DataBlock
    {
        
    } DataBlock;
