const PAGE_SIZE = 4096;
const NAME_LEN = 32;
const IP_LEN = 16;

typedef char host_t[IP_LEN];

struct page_t{
	unsigned int size;
	char addr[PAGE_SIZE];
};

struct pageid_t{
	char name[NAME_LEN];
	unsigned int size;
};

struct request_t{
	pageid_t pageid;
	mode_t mode;
	char from[IP_LEN]
};

enum mode_t{NA, RO, RW};


program PSU_DSM {
    version PSU_DSM_VERS {
        int PSU_DSM_PAGE_FIND(pageid_t ) = 1;
	host_t PSU_DSM_PAGE_LOCATE(pageid_t) = 2;
	int PSU_DSM_PAGE_CREAT(pageid_t) = 3;
	int PSU_DSM_PAGE_UPDATE(request_t) = 4;
	page_t PSU_DSM_PAGE_REQUEST(request_t) = 5;
	page_t PSU_DSM_PAGE_FETCH(request_t) = 6;
	void PSU_DSM_PAGE_ACK(pageid_t) = 7;
    } = 1;
} = 0x20002234;
