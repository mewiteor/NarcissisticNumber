import 水仙花数
import time
import multiprocessing
import humanfriendly

def sub(位数和进制):
    位数,进制 = 位数和进制
    st = time.time()
    结果 = 水仙花数.指定进制和位数时所有的水仙花数(进制, 位数)
    ed = time.time()
    print(f'{位数}: {ed - st} s')
    return 结果

def main():
    进制 = int(input("进制:"))
    st = time.time()
    最大位数 = 水仙花数.获取最大位数(进制)
    print(f"最大位数: {最大位数}")

    res = []
    with multiprocessing.Pool() as pool:
        for r in pool.imap_unordered(sub, [(i, 进制) for i in range(2, 最大位数 + 1)]):
            res += r
    res = sorted(res, key=lambda x: (len(x), x))
    for x in res:
        print(x)
    ed = time.time()
    print('总耗时: ' + humanfriendly.format_timespan(ed - st))

if __name__=='__main__':
    main()