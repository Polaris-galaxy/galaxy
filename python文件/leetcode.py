class Solution(object):
    def groupAnagrams(self, strs):

        char_int = []
        num_index = []
        
        for i in range(len(strs)):
            num = []
            char = []
            for j in range(len(strs[i])):
                if strs[i][j] not in char:
                    char.append(strs[i][j])
                    num.append(1)
                else:
                    index = char.index(strs[i][j])
                    num[index] += 1
            char_int.append([char, num])
            for k in range(0, i):
                if char_int[i] == char_int[k]:
                    num_index.append(k)
                    print(strs[k])
        return None  
if __name__ == "__main__":
    strs = ["eat","tea","tan","ate","nat","bat"]
    object.groupAnagrams(None, strs)