<template>
  <div class="user-center">
    <el-card>
      <h2>用户中心</h2>

      <el-form :model="form" label-width="100px">
        <!-- ================== 所有用户基本信息 ================== -->
        <el-form-item label="用户名">
          <el-input v-model="form.username" disabled />
        </el-form-item>

        <el-form-item label="年龄">
          <el-input v-model="form.age" />
        </el-form-item>

        <el-form-item label="手机号">
          <el-input v-model="form.phone" />
        </el-form-item>

        <el-form-item label="身份证号">
          <el-input v-model="form.id_card" />
        </el-form-item>

        <!-- ================== 用餐者专属信息 ================== -->
        <template v-if="isDiner">
          <el-divider content-position="left">用餐者信息</el-divider>

          <!-- 饮食习惯 -->
          <el-form-item label="饮食偏好">
            <el-select v-model="form.taste_preference" placeholder="请选择">
              <el-option label="清淡" value="清淡" />
              <el-option label="低糖" value="低糖" />
              <el-option label="低脂" value="低脂" />
              <el-option label="咸口" value="咸口" />
              <el-option label="辣口" value="辣口" />
            </el-select>
          </el-form-item>

          <!-- 疾病史 -->
          <el-form-item label="疾病史(如有)">
            <el-select v-model="form.disease_history" placeholder="请选择">
              <el-option label="无" value="无" />
              <el-option label="高血压" value="高血压" />
              <el-option label="糖尿病" value="糖尿病" />
            </el-select>
          </el-form-item>

          <!-- 家庭选择 -->
          <el-form-item label="所属家庭">
            <el-row :gutter="10">
              <el-col :span="18">
                <el-select v-model="form.family_id" placeholder="请选择家庭">
                  <el-option
                    v-for="item in familyList"
                    :key="item.family_id"
                    :label="item.family_name + ' (ID:' + item.family_id + ')'"
                    :value="item.family_id"
                  />
                </el-select>
              </el-col>
              <el-col :span="6">
                <el-button type="primary" @click="openCreateFamilyDialog">创建家庭</el-button>
              </el-col>
            </el-row>
          </el-form-item>

          <!-- 区域选择 -->
          <el-form-item label="所属区域">
            <el-select v-model="form.region_id" placeholder="请选择区域">
              <el-option
                v-for="item in regionList"
                :key="item.region_id"
                :label="item.region_name + ' (ID:' + item.region_id + ')'"
                :value="item.region_id"
              />
            </el-select>
          </el-form-item>
        </template>

        <!-- ================== 食堂管理者专属信息 ================== -->
        <template v-if="isCanteenAdmin">
          <el-divider content-position="left">食堂管理者信息</el-divider>

          <el-form-item label="负责餐厅">
            <el-input v-model="form.canteen_name" disabled />
          </el-form-item>
        </template>

        <!-- ================== 管理员专属信息 ================== -->
        <template v-if="isAdmin">
          <el-divider content-position="left">管理员信息</el-divider>

          <el-form-item label="管理等级">
            <el-input v-model="form.admin_level" disabled />
          </el-form-item>

          <el-form-item label="管理区域">
            <el-input v-model="form.admin_region" disabled />
          </el-form-item>
        </template>

        <el-form-item>
          <el-button type="primary" @click="updateUser">
            保存修改
          </el-button>
          <el-button type="warning" @click="openChangePasswordDialog">
            修改密码
          </el-button>
        </el-form-item>
      </el-form>

      <!-- 修改密码对话框 -->
      <el-dialog
        v-model="changePasswordDialogVisible"
        title="修改密码"
        width="500px"
      >
        <el-form :model="changePasswordForm" label-width="100px" :rules="changePasswordRules" ref="changePasswordFormRef">
          <el-form-item label="旧密码" prop="oldPassword">
            <el-input v-model="changePasswordForm.oldPassword" type="password" placeholder="请输入旧密码" />
          </el-form-item>
          <el-form-item label="新密码" prop="newPassword">
            <el-input v-model="changePasswordForm.newPassword" type="password" placeholder="请输入新密码（长度≥6）" />
          </el-form-item>
          <el-form-item label="确认新密码" prop="confirmPassword">
            <el-input v-model="changePasswordForm.confirmPassword" type="password" placeholder="请再次输入新密码" />
          </el-form-item>
        </el-form>
        <template #footer>
          <span class="dialog-footer">
            <el-button @click="changePasswordDialogVisible = false">取消</el-button>
            <el-button type="primary" @click="changePassword">修改</el-button>
          </span>
        </template>
      </el-dialog>

      <!-- 创建家庭对话框 -->
      <el-dialog
        v-model="createFamilyDialogVisible"
        title="创建家庭"
        width="500px"
      >
        <el-form :model="createFamilyForm" label-width="100px">
          <el-form-item label="家庭名称">
            <el-input v-model="createFamilyForm.family_name" placeholder="请输入家庭名称" />
          </el-form-item>
        </el-form>
        <template #footer>
          <span class="dialog-footer">
            <el-button @click="createFamilyDialogVisible = false">取消</el-button>
            <el-button type="primary" @click="createFamily">创建</el-button>
          </span>
        </template>
      </el-dialog>
    </el-card>
  </div>
</template>

<script setup>
import { ref, onMounted, computed } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const user = ref({
  user_id: '',
  username: '',
  role: ''
})

onMounted(() => {
  const u = localStorage.getItem('user')
  if (u) {
    user.value = JSON.parse(u)
  }
  getUserInfo()
  getFamilyList()
  getRegionList()
})

const isDiner = computed(() => user.value.role === 'diner')
const isCanteenAdmin = computed(() => user.value.role === 'canteen_manager')
const isAdmin = computed(() => user.value.role === 'admin' || user.value.role === 'system_admin')

const form = ref({
  user_id: '',
  username: '',
  age: '',
  phone: '',
  id_card: '',
  address: '',
  taste_preference: '',
  disease_history: '',
  family_id: '',
  family_name: '',
  region_id: '',
  region_name: '',
  canteen_id: '',
  canteen_name: '',
  admin_level: '',
  admin_region: ''
})

const familyList = ref([])
const regionList = ref([])

const createFamilyDialogVisible = ref(false)
const createFamilyForm = ref({
  family_name: ''
})

const changePasswordDialogVisible = ref(false)
const changePasswordForm = ref({
  oldPassword: '',
  newPassword: '',
  confirmPassword: ''
})
const changePasswordFormRef = ref(null)

const changePasswordRules = {
  oldPassword: [
    { required: true, message: '请输入旧密码', trigger: 'blur' }
  ],
  newPassword: [
    { required: true, message: '请输入新密码', trigger: 'blur' },
    { min: 6, message: '新密码长度至少为6位', trigger: 'blur' }
  ],
  confirmPassword: [
    { required: true, message: '请确认新密码', trigger: 'blur' },
    {
      validator: (rule, value, callback) => {
        if (value !== changePasswordForm.value.newPassword) {
          callback(new Error('两次输入的密码不一致'))
        } else {
          callback()
        }
      },
      trigger: 'blur'
    }
  ]
}

axios.interceptors.request.use(config => {
  const token = localStorage.getItem('token')
  if (token) {
    config.headers.Authorization = 'Bearer ' + token
  }
  return config
})

const getUserInfo = async () => {
  try {
    if (!user.value || !user.value.user_id) {
      ElMessage.error('请先登录')
      return
    }

    const res = await axios.get('http://192.168.56.100:8080/userCenter', {
      params: { user_id: user.value.user_id }
    })

    const data = res.data.data

    form.value = {
      user_id: data.user_id,
      username: data.username,
      age: data.age,
      phone: data.phone,
      id_card: data.id_card,
      address: data.address,
      taste_preference: data.tastePreference || '',
      disease_history: data.diseaseHistory || '',
      family_id: data.familyId || '',
      family_name: data.familyName || '',
      region_id: data.regionId || '',
      region_name: data.regionName || '',
      canteen_id: data.canteenId || '',
      canteen_name: data.canteenName || '',
      admin_level: data.adminLevel || '',
      admin_region: data.adminRegion || ''
    }
  } catch (err) {
    ElMessage.error('获取用户信息失败')
    console.error(err)
  }
}

const getFamilyList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/familyList')
    familyList.value = res.data.data || []
  } catch (err) {
    ElMessage.error('获取家庭列表失败')
  }
}

const getRegionList = async () => {
  try {
    const res = await axios.get('http://192.168.56.100:8080/regionList')
    regionList.value = res.data.data || []
  } catch (err) {
    ElMessage.error('获取区域失败')
  }
}

const updateUser = async () => {
  try {
    await axios.post('http://192.168.56.100:8080/userCenterUpdate', {
      user_id: form.value.user_id,
      age: form.value.age,
      phone: form.value.phone,
      address: form.value.address,
      id_card: form.value.id_card,
      taste_preference: form.value.taste_preference,
      disease_history: form.value.disease_history,
      family_id: form.value.family_id,
      region_id: form.value.region_id
    })

    ElMessage.success('更新成功')
  } catch (err) {
    ElMessage.error('更新失败：' + (err.response?.data?.msg || '服务器异常'))
    console.error(err)
  }
}

const openCreateFamilyDialog = () => {
  createFamilyForm.value.family_name = ''
  createFamilyDialogVisible.value = true
}

const createFamily = async () => {
  try {
    if (!user.value || !user.value.user_id) {
      ElMessage.error('请先登录')
      return
    }

    if (!createFamilyForm.value.family_name.trim()) {
      ElMessage.error('请输入家庭名称')
      return
    }

    const res = await axios.post('http://192.168.56.100:8080/createFamily', {
      user_id: user.value.user_id,
      family_name: createFamilyForm.value.family_name
    })

    if (res.data.code === 0) {
      ElMessage.success('创建家庭成功')
      createFamilyDialogVisible.value = false
      getFamilyList()
      getUserInfo()
    } else {
      ElMessage.error('创建家庭失败：' + res.data.message)
    }
  } catch (err) {
    ElMessage.error('创建家庭失败：' + (err.response?.data?.message || '服务器异常'))
    console.error(err)
  }
}

const openChangePasswordDialog = () => {
  changePasswordForm.value = {
    oldPassword: '',
    newPassword: '',
    confirmPassword: ''
  }
  changePasswordDialogVisible.value = true
}

const changePassword = async () => {
  try {
    if (!user.value || !user.value.user_id) {
      ElMessage.error('请先登录')
      return
    }

    if (!changePasswordFormRef.value) return
    await changePasswordFormRef.value.validate()

    const res = await axios.post('http://192.168.56.100:8080/changePassword', {
      user_id: user.value.user_id,
      old_password: changePasswordForm.value.oldPassword,
      new_password: changePasswordForm.value.newPassword
    })

    if (res.data.code === 0) {
      ElMessage.success('密码修改成功，即将跳转到登录页')
      changePasswordDialogVisible.value = false

      localStorage.removeItem('user')
      localStorage.removeItem('token')

      setTimeout(() => {
        window.location.href = '/login'
      }, 1500)
    } else {
      ElMessage.error('密码修改失败：' + (res.data.msg || '旧密码错误或新密码长度不足'))
    }
  } catch (err) {
    if (err.name === 'Error') {
      return
    }
    ElMessage.error('密码修改失败：' + (err.response?.data?.msg || '服务器异常'))
    console.error(err)
  }
}
</script>

<style scoped>
.user-center {
  padding: 20px;
}
</style>
