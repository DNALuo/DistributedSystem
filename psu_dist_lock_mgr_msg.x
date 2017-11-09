struct RequestPack{
    int lock_number;
    int seqno;
    string nodeip<17>;
};

program PSU_DIST_LOCK_MGR {
  version PSU_DIST_LOCK_MGR_V1 {
    void INIT_LOCK_MGR(string) = 1;
    void ACQUIRE_LOCK(int) = 2;
    void RELEASE_LOCK(int) = 3;
    void REQUEST(RequestPack) = 4;
  } = 1;
} = 0x2ffffff;

