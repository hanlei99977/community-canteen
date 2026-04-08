<template>
  <div>

    <!-- 标题 + 新建 -->
    <div style="display:flex;justify-content: space-between;align-items: center;margin-bottom: 20px;">
      <h2>菜品管理</h2>
      <el-button type="primary" @click="openDialog">新增菜品</el-button>
    </div>

    <!-- 菜品表 -->
    <el-table :data="dishList" border>

      <el-table-column prop="name" label="菜名"/>
      <el-table-column prop="type" label="类型"/>
      <el-table-column prop="price" label="价格"/>
      <el-table-column prop="calories" label="热量"/>
      <el-table-column prop="nutrition_info" label="营养信息"/>
      <el-table-column label="操作" width="150">
        <template #default="scope">
          <el-button
            type="danger"
            size="small"
            @click="deleteDish(scope.row.dish_id)"
          >
            下架
          </el-button>
        </template>
      </el-table-column>

    </el-table>

    <!-- 新增弹窗 -->
    <el-dialog v-model="dialogVisible" title="新增菜品" width="500px">

      <el-form>

        <el-form-item label="菜名">
          <el-input v-model="form.name"/>
        </el-form-item>

        <el-form-item label="类型">
          <el-select v-model="form.type">
            <el-option label="荤" value="荤"/>
            <el-option label="素" value="素"/>
          </el-select>
        </el-form-item>

        <el-form-item label="价格">
          <el-input v-model="form.price" type="number"/>
        </el-form-item>

        <el-form-item label="热量">
          <el-input v-model="form.calories" type="number"/>
        </el-form-item>

        <el-form-item label="营养信息">
          <el-input v-model="form.nutrition_info"/>
        </el-form-item>

      </el-form>

      <template #footer>
        <el-button @click="dialogVisible = false">取消</el-button>
        <el-button type="primary" @click="submit">确定</el-button>
      </template>

    </el-dialog>

  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

// 用户
const getUser = () => JSON.parse(localStorage.getItem("user"))

// 数据
const dishList = ref([])
const dialogVisible = ref(false)

// 表单
const form = ref({
  canteen_id: '',
  name: '',
  type: '',
  price: 0.0,
  calories: 0,
  nutrition_info: ''
})

// ================= 获取菜品 =================
const getDishes = async () => {
  const user = getUser()

  const res = await axios.get('http://192.168.56.100:8080/getDishes', {
    params: { canteen_id: user.canteen_id }
  })

  dishList.value = res.data.data
}

// ================= 打开弹窗 =================
const openDialog = () => {
  const user = getUser()

  dialogVisible.value = true

  form.value = {
    canteen_id: user.canteen_id,
    name: '',
    type: '',
    price: 0.0,
    calories: 0,
    nutrition_info: ''
  }
}

// ================= 新增 =================
const submit = async () => {

  if (!form.value.name || !form.value.type || !form.value.price) {
    ElMessage.warning("请填写完整信息")
    return
  }

  form.value.price = Number(form.value.price)
  form.value.calories = parseInt(form.value.calories)
  
  const res = await axios.post(
    'http://192.168.56.100:8080/dishCreate',
    form.value,
  )

  if (res.data.code === 0) {
    ElMessage.success("新增成功")
    dialogVisible.value = false
    getDishes()
  } else {
    ElMessage.error(res.data.msg || "新增失败")
  }
}

// ================= 下架 =================
const deleteDish = async (dish_id) => {

  await axios.post(
    'http://192.168.56.100:8080/dishDisable',
    { dish_id }
  )

  ElMessage.success("下架成功")
  getDishes()
}

// 初始化
onMounted(() => {
  getDishes()
})
</script>