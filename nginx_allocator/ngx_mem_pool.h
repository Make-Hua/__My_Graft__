#pragma once
#include <cstdlib>
#include <memory.h>

const int _SIZE = 128;
using u_char = unsigned char;
using ngx_uint_t = unsigned int;

// 类型前置声明
struct ngx_pool_s;

// 清理函数的类型（回调函数）
typedef void (*ngx_pool_cleanup_pt)(void* data);
struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt      handler;                             // 定义一个函数指针，保存清理操作的回调函数
    void                     *data;                               // 回调参数
    ngx_pool_cleanup_s       *next;                               // 所有 cleanup 操作串在一条链表上
}; 

struct ngx_pool_large_s {
    ngx_pool_large_s         *next;
    void                     *alloc;
};

// 分配小块内存头部数据信息
struct ngx_pool_data_t {
    u_char                   *last;                               // 小块内存池可用内存的起始地址
    u_char                   *end;                                // 小块内存池可用内存的末尾地址
    ngx_pool_s               *next;                               // 指向下一小块内存
    ngx_uint_t               failed;                              // 内存分配失败的次数，失败超过3次将执行其他函数
} ;

// ngx 内存池的头部信息和管理成员
struct ngx_pool_s {
    ngx_pool_data_t          d;                                   // 存储当前小块内存池的使用情况
    size_t                   max;                                 // 小块内存和大块内存的分界线
    ngx_pool_s               *current;                            // 指向第一个提供小块内存分配的小块内存池
    ngx_pool_large_s         *large;                              // 大块内存链表的入口
    ngx_pool_cleanup_s       *cleanup;                            //回调清理函数入口
};


/*
 * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
// 把 b 调整到 a 的倍数
#define ngx_align(d, a) (((d) + (a - 1)) & ~(a - 1))
// 把指针 p 调整到 a 的临近倍数
#define ngx_align_ptr(p, a) (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))
#define NGX_ALIGNMENT sizeof(unsigned long)
#define ngx_memzero(buf, n) (void) memset(buf, 0, n)

const int ngx_pagesize = 4096;                                    // 物理页面大小（4K）
const int NGX_MAX_ALLOC_FROM_POOL = ngx_pagesize - 1;             // 内存池最大可分配大小   需要预存一个指针的空间
const int NGX_DEFAULT_POOL_SIZE = 16 * 1024;                      // 16k
const int NGX_POOL_ALIGNMENT = 16;                                // 按照16字节对齐
const int NGX_MIN_POOL_SIZE = 
                ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)),
                    NGX_POOL_ALIGNMENT);                          // 

/* OOP 实现 nginx 内存池代码 */
class ngx_mem_pool {
public:
    ngx_mem_pool(size_t size = _SIZE);
    ~ngx_mem_pool();

    void* ngx_create_pool(size_t size);                           // 创建size大小内存池
    void* ngx_palloc(size_t size);                                // 从内存池申请 size 大小内存（考虑了字节对齐）
    void* ngx_pnalloc(size_t size);                               // 从内存池申请 size 大小内存（不考虑字节对齐）
    void* ngx_pcalloc(size_t size);                               // 调用的就是 ngx_palloc 实现内存分配，但是会初始化为0
    void ngx_pfree(void* p);                                      // 大块内存释放
    void ngx_reset_pool();                                        // 内存重置函数
    void ngx_destroy_pool();                                      // 内存销毁函数
    ngx_pool_cleanup_s*
        ngx_pool_cleanup_add(size_t size);                        // 添加回调清理操作函数

private:
    ngx_pool_s* pool_;                                            // 指向内存池入口 

    void* ngx_palloc_small(size_t size, ngx_uint_t align);        // 小块内存分配
    void* ngx_palloc_large(size_t size);                          // 大块内存分配
    void* ngx_palloc_block(size_t size);                          // 分配新的小块内存池
};