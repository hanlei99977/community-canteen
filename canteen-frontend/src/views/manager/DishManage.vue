<template>
  <div>

    <!-- 标题 + 新建 -->
    <div style="display:flex;justify-content: space-between;align-items: center;margin-bottom: 20px;">
      <h2>菜品管理</h2>
      <el-button type="primary" @click="openDialog">新增菜品</el-button>
    </div>

    <!-- Tab -->
    <el-tabs v-model="activeTab">

      <el-tab-pane label="在售菜品" name="on"/>
      <el-tab-pane label="已下架菜品" name="off"/>

    </el-tabs>

    <!-- 表格 -->
    <el-table
      :data="activeTab === 'on' ? onDishList : offDishList"
      border
    >

      <el-table-column prop="name" label="菜名"/>
      <el-table-column prop="type" label="类型"/>
      <el-table-column prop="price" label="价格"/>
      <el-table-column prop="calories" label="热量"/>
      <el-table-column prop="nutrition_info" label="营养信息"/>

      <el-table-column label="操作" width="150">
        <template #default="scope">

          <el-button
            v-if="activeTab === 'on'"
            type="warning"
            size="small"
            @click="disableDish(scope.row.dish_id)"
          >
            下架
          </el-button>

          <el-button
            v-else
            type="success"
            size="small"
            @click="enableDish(scope.row.dish_id)"
          >
            上架
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
import { ref, onMounted, computed } from 'vue'
import axios from 'axios'
import { ElMessage } from 'element-plus'

const getUser = () => JSON.parse(localStorage.getItem("user"))

const activeTab = ref("on")

// ⭐ 所有数据
const allDishList = ref([])

// ⭐ 分组
const onDishList = computed(() =>
  allDishList.value.filter(d => d.status === 1)
)

const offDishList = computed(() =>
  allDishList.value.filter(d => d.status === 0)
)

const dialogVisible = ref(false)

const form = ref({
  canteen_id: '',
  name: '',
  type: '',
  price: 0.0,
  calories: 0,
  nutrition_info: ''
})

// ================= 获取全部菜品 =================
const getDishes = async () => {
  const user = getUser()

  const res = await axios.get('http://192.168.56.100:8080/getDishes', {
    params: { canteen_id: user.canteen_id }
  })

  allDishList.value = res.data.data
  console.log(res.data.data)
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
    form.value
  )

  if (res.data.code === 0) {
    ElMessage.success("新增成功")
    dialogVisible.value = false
    getDishes()
  }
}

// ================= 下架 =================
const disableDish = async (dish_id) => {
  await axios.post('http://192.168.56.100:8080/dishDisable', { dish_id })
  ElMessage.success("已下架")
  getDishes()
}

// ================= 上架 =================
const enableDish = async (dish_id) => {
  await axios.post('http://192.168.56.100:8080/dishEnable', { dish_id })
  ElMessage.success("已上架")
  getDishes()
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

onMounted(() => {
  getDishes()
})
</script>