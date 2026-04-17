<template>
  <div class="purchase">
    <el-card>
      <h2>日常采购</h2>

      <el-form :model="purchaseForm" label-width="100px" :rules="rules" ref="purchaseFormRef">
        <el-form-item label="采购金额" prop="amount">
          <el-input v-model.number="purchaseForm.amount" type="number" placeholder="请输入采购金额" />
        </el-form-item>

        <el-form-item label="采购时间" prop="purchase_date">
          <el-date-picker
            v-model="purchaseForm.purchase_date"
            type="date"
            placeholder="选择采购日期"
            style="width: 100%"
            value-format="YYYY-MM-DD"
            format="YYYY-MM-DD"
          />
        </el-form-item>

        <el-form-item label="备注" prop="remark">
          <el-input
            v-model="purchaseForm.remark"
            type="textarea"
            placeholder="请输入备注信息"
            rows="4"
          />
        </el-form-item>

        <el-form-item>
          <el-button type="primary" @click="submitPurchase">
            提交采购
          </el-button>
        </el-form-item>
      </el-form>

      <el-divider />

      <h3>采购记录</h3>
      <el-table :data="purchaseList" style="width: 100%">
        <el-table-column prop="bill_id" label="采购单ID" width="100" />
        <el-table-column prop="amount" label="采购金额" width="120" />
        <el-table-column prop="purchase_date" label="采购时间" width="180">
          <template #default="scope">
            {{ formatDate(scope.row.purchase_date) }}
          </template>
        </el-table-column>
        <el-table-column prop="remark" label="备注" />
      </el-table>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted, reactive } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

// 采购表单
const purchaseForm = reactive({
  amount: '',
  purchase_date: '',
  remark: ''
})

// 表单验证规则
const rules = {
  amount: [
    { required: true, message: '请输入采购金额', trigger: 'blur' },
    { type: 'number', min: 0.01, message: '采购金额必须大于0', trigger: 'blur' }
  ],
  purchase_date: [
    { required: true, message: '请选择采购时间', trigger: 'change' }
  ]
}

// 表单引用
const purchaseFormRef = ref(null)

// 采购记录列表
const purchaseList = ref([])

// 格式化日期
const formatDate = (dateStr) => {
  if (!dateStr) return ''
  return dateStr.slice(0, 10)
}

// 请求头自动携带 token
axios.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = 'Bearer ' + token
  }
  return config
})

// 获取采购记录
const getPurchaseRecords = async () => {
  try {
    const user = JSON.parse(localStorage.getItem('user'))
    if (!user || !user.user_id) {
      ElMessage.error('请先登录')
      return
    }

    const canteenRes = await axios.get('http://192.168.56.100:8080/myCanteen', {
      params: { user_id: user.user_id }
    })

    if (canteenRes.data.code !== 0) {
      ElMessage.error('获取食堂信息失败：' + canteenRes.data.message)
      return
    }

    const canteen_id = canteenRes.data.data.id

    const res = await axios.get('http://192.168.56.100:8080/purchaseList', {
      params: { canteen_id }
    })

    if (res.data.code === 0) {
      purchaseList.value = res.data.data
    } else {
      ElMessage.error('获取采购记录失败：' + res.data.message)
    }
  } catch (err) {
    ElMessage.error('获取采购记录失败')
  }
}

// 提交采购
const submitPurchase = async () => {
  try {
    await purchaseFormRef.value.validate()

    const user = JSON.parse(localStorage.getItem('user'))

    const canteenRes = await axios.get('http://192.168.56.100:8080/myCanteen', {
      params: { user_id: user.user_id }
    })

    const canteen_id = canteenRes.data.data.id

    const res = await axios.post('http://192.168.56.100:8080/createPurchase', {
      canteen_id,
      amount: purchaseForm.amount,
      purchase_date: purchaseForm.purchase_date,
      remark: purchaseForm.remark
    })

    if (res.data.code === 0) {
      ElMessage.success('采购记录提交成功')

      purchaseForm.amount = ''
      purchaseForm.purchase_date = ''
      purchaseForm.remark = ''

      getPurchaseRecords()
    } else {
      ElMessage.error(res.data.message)
    }
  } catch (err) {}
}

onMounted(() => {
  getPurchaseRecords()
})
</script>

<style scoped>
.purchase {
  padding: 20px;
}
</style>