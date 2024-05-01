#include"stdafx.h"
#include<vector>
#include<algorithm>
#include "zybkLog.h"

using namespace std;

class Solution {
public:
	int minSkips(vector<int> dist, int speed, int hoursBefore) {
   	int n = dist.size();
        int max = speed * hoursBefore;
        vector<vector<int>> dp(n,vector<int>(n));
        vector<int> distF(n);
        for(int j = 0; j < n; j++) {
            distF[j] = dist[j] + abs((speed - dist[j]) % speed);
			dp[0][0] = dist[0];
            if (j > 0)
			{
				dp[0][j] = dist[0];
            	dp[j][0] = dp[j - 1][0] + abs((speed - (dp[j - 1][0] % speed)) % speed) + dist[j];
				LOGD("dp[%d][%d]= %d", j, 0, dp[j][0]);
			}
        }
        if (dp[n - 1][0] <= max) {
            return 0;
        }
        for(int j = 1; j < n; j++) {
            for(int i = 1; i < n; i++) {
                // dp[i][j] = (dp[i - 1][j] + distF[i - 1]) < (dp[i - 1][j - 1] + dist[i -1]) ?  (dp[i - 1][j] + distF[i -1]) : (dp[i - 1][j - 1] + dist[i -1]);
                dp[i][j] = (dp[i - 1][j] + abs((speed - dp[i - 1][j]) % speed) + dist[i]) < (dp[i - 1][j - 1] + dist[i]) ?  (dp[i - 1][j] +  abs((speed - dp[i - 1][j]) % speed) + dist[i]) : (dp[i - 1][j - 1] + dist[i]);
				LOGD("dp[%d][%d]= %d", i, j, dp[i][j]);
			}
            if (dp[n - 1][j] <= max) {
                return j;
            }
        }
        return -1;
    }
}; 