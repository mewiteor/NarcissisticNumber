from dataclasses import dataclass
import math

def 获取最大位数(进制: int):
    r'''
    获取最大位数
    参数: 进制: >=2 的整数
    原理:
        |_ log_进制 {(进制-1)^位数 * 位数} _| + 1 >= 位数
        比如: 10进制, n位数最大水仙花数可能为:
            999...9 == 9^n + 9^n + 9^n + ... + 9^n = 9^n * n
            \_  __/    \__________  ____________/
              \/                  \/
             n个9                n个9^n
        只有9^n * n达到n位数时, 才可能等于n位数的左边
    '''
    assert 进制 >= 2
    位数 = 1
    while int(math.log((进制-1) ** 位数 * 位数) / math.log(进制)) + 1 >= 位数:
        位数 += 1
    return 位数 - 1

def 指定进制和位数时所有的水仙花数(进制: int, 位数: int) -> list[str]:
    结果 = []
    '''
    从0到进制,每个数字在测试的数中的数量
    比如，对于 112333，此数组值为[0, 2, 1, 3, 0, ...]
    '''
    各数的数量 = [0] * 进制

    def ToBase(v: int):
        r = []
        while v:
            r.append(v % 进制)
            v //= 进制
        s = []
        for x in r[::-1]:
            if x < 10:
                s.append(chr(ord('0') + x))
            elif x < 36:
                s.append(chr(ord('A') + x - 10))
            else:
                assert False
        return ''.join(s)

    def Check(s: int):
        t = [0] * 进制
        ss = s
        while ss > 0:
            t[ss % 进制] += 1
            ss //= 进制
        if 各数的数量 == t:
            结果.append(ToBase(s))

    def PreCheck(cur: int, remain: int, s: int) -> bool:
        if s == 0:
            return True
        if int(math.log(s) / math.log(进制)) + 1 < 位数:
            return True
        max_v = s + cur ** 位数 * remain
        max_t = []
        s_t = []
        ss = s
        for _ in range(位数):
            max_t.append(max_v % 进制)
            s_t.append(ss % 进制)
            max_v //= 进制
            ss //= 进制
        if max_t[位数 - 1] != s_t[位数 - 1]:
            # 可能所有位的数都变了
            return True
        
        for i in range(位数 - 1, 0, -1):
            lastSameDigit = i
            if max_t[i - 1] != s_t[i - 1]:
                break
        else:
            lastSameDigit = 0
        t = [0] * 进制
        for i in range(位数, lastSameDigit, -1):
            t[max_t[i - 1]] += 1
        
        for i in range(进制 - 1, cur, -1):
            if t[i] > 各数的数量[i]:
                return False
        return remain >= sum(t[:cur + 1])

    def SubSearch(cur: int, remain: int, s: int):
        if remain == 0:
            Check(s)
        elif cur == 0:
            各数的数量[0] = remain
            Check(s)
            各数的数量[0] = 0
        elif PreCheck(cur, remain, s):
            # 找到满足条件的最大nstart: sum < base^digit-cur^digit*nstart
            nstart = min(math.floor((进制 ** 位数 - 1 - s) / cur ** 位数), remain)

            # 找到满足条件的最小nend: sum>=base^(digit-1)-cur^digit*nend-(cur-1)^digit*(remain-nend)
            nend = max(math.ceil(((进制**(位数-1)-s-(cur-1)**位数*remain)) / (cur**位数-(cur-1)**位数)), 0)

            for i in range(nstart, nend - 1, -1):
                各数的数量[cur] = i
                if i == 0:
                    # 未选择 cur 这个数字
                    SubSearch(cur - 1, remain, s)
                else:
                    # 选择了 i 次 cur 这个数字
                    SubSearch(cur - 1, remain - i, s + cur ** 位数 * i)

    SubSearch(进制 - 1, 位数, 0)
    return 结果
