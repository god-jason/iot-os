local EKF = {}
EKF.__index = EKF

-- 初始化 EKF
function EKF:new(state_dim, meas_dim)
    local obj = {
        x = {},      -- 状态向量
        P = {},      -- 协方差矩阵
        Q = {},      -- 过程噪声
        R = {},      -- 观测噪声
        F = {},      -- 状态转移雅可比
        H = {},      -- 观测雅可比
        I = {},      -- 单位矩阵
        state_dim = state_dim,
        meas_dim = meas_dim
    }
    -- 初始化单位矩阵
    for i = 1,state_dim do
        obj.I[i] = {}
        for j = 1,state_dim do
            obj.I[i][j] = (i==j) and 1 or 0
        end
    end
    setmetatable(obj, self)
    return obj
end

-- 矩阵乘法
local function matmul(A,B)
    local m=#A
    local n=#B[1]
    local p=#B
    local C = {}
    for i=1,m do
        C[i] = {}
        for j=1,n do
            C[i][j] = 0
            for k=1,p do
                C[i][j] = C[i][j] + A[i][k]*B[k][j]
            end
        end
    end
    return C
end

-- EKF 预测步骤
function EKF:predict(f_func, F_jacobian, u)
    -- 状态预测
    self.x = f_func(self.x, u)
    -- 协方差预测
    self.F = F_jacobian(self.x,u)
    self.P = matadd(matmul(matmul(self.F,self.P),transpose(self.F)), self.Q)
end

-- EKF 更新步骤
function EKF:update(z, h_func, H_jacobian)
    local H = H_jacobian(self.x)
    local y = matsub(z, h_func(self.x))        -- 观测残差
    local S = matadd(matmul(matmul(H,self.P),transpose(H)), self.R)
    local K = matmul(matmul(self.P,transpose(H)), inv(S)) -- 卡尔曼增益
    self.x = matadd(self.x, matmul(K, y))
    self.P = matmul(matsub(self.I, matmul(K,H)), self.P)
end

return EKF