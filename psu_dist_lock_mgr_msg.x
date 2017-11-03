program PSU_DIST_LOCK_MGR {
  version PSU_DIST_LOCK_MGR_V1 {
    void INIT_LOCK_MGR(string) = 1;
    void ACQUIRE_LOCK(int) = 2;
    void RELEASE_LOCK(int) = 3;
  } = 1;
} = 0x2ffffff;

