<template>
  <div>

    <!-- 标题 + 新建 -->
    <div style="display:flex;justify-content: space-between;align-items: center;margin-bottom: 20px;">
      <h2>每日餐单管理</h2>
      <el-button type="primary" @click="openDialog">新建</el-button>
    </div>

    <!-- ================= 餐单列表 ================= -->
    <el-table :data="menuList" border>

      <el-table-column prop="date" label="日期" width="120"/>
      <el-table-column prop="meal_type" label="餐别" width="100"/>

      <el-table-column label="餐单菜品">        <template #default="scope">
          <span v-if="scope.row.dishes && scope.row.dishes.length">
            <span v-for="d in scope.row.dishes" :key="d.dish_id">
              {{ d.name }}（{{ d.price }}元）、
            </span>
          </span>
          <span v-else>暂无菜品</span>
        </template>
      </el-table-column>

      <el-table-column label="操作" width="120">
        <template #default="scope">
          <el-button type="danger" size="small" @click="deleteMenu(scope.row.menu_id)">
            删除
          </el-button>
        </template>
      </el-table-column>

    </el-table>

    <!-- ================= 新建弹窗 ================= -->
    <el-dialog v-model="dialogVisible" title="新建餐单" width="500px">

      <el-form>

        <el-form-item label="日期">
          <el-date-picker
            v-model="form.date"
            type="date"
            value-format="YYYY-MM-DD"
          />
        </el-form-item>

        <el-form-item label="餐别">
          <el-select v-model="form.meal_type">
            <el-option label="午餐" value="午餐"/>
            <el-option label="晚餐" value="晚餐"/>
          </el-select>
        </el-form-item>

        <!-- ⭐ 所有菜品（关键点） -->
          <span v-if="availableDishList.length === 0">
            当前没有可用菜品，请先添加或上架
          </span>

        <el-form-item label="菜品">
          <el-checkbox-group v-model="form.dish_ids">
            <el-checkbox
              v-for="dish in availableDishList"
              :key="dish.dish_id"
              :label="Number(dish.dish_id)"
            >
              {{ dish.name }}（{{ dish.price }}元）
            </el-checkbox>
          </el-checkbox-group>
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

// 用户
const getUser = () => JSON.parse(localStorage.getItem("user"))

const availableDishList = computed(() =>
  dishList.value.filter(d => Number(d.status) === 1)
)

// ================= 数据 =================
const menuList = ref([])
const dishList = ref([])
const dialogVisible = ref(false)

// 表单
const form = ref({
  canteen_id: '',
  date: '',
  meal_type: '',
  dish_ids: []
})

// ================= 获取餐单（带菜品） =================
const getMenus = async () => {
  const user = getUser()

  const res = await axios.get('http://192.168.56.100:8080/getMenus', {
    params: { canteen_id: user.canteen_id }
  })

  menuList.value = res.data.data
}

// ================= 获取所有菜品 =================
const getDishes = async () => {
  const user = getUser()

  const res = await axios.get('http://192.168.56.100:8080/getDishes', {
    params: { canteen_id: user.canteen_id }
  })

  console.log("dishList:", res.data.data) // 调试

  dishList.value = res.data.data
}

// ================= 打开弹窗 =================
const openDialog = () => {
  const user = getUser()

  dialogVisible.value = true

  form.value = {
    canteen_id: user.canteen_id,
    date: '',
    meal_type: '',
    dish_ids: []
  }
}

// ================= 提交 =================
const submit = async () => {

  // 防止 null
  form.value.dish_ids = form.value.dish_ids.filter(id => id != null)

  if (!form.value.date || !form.value.meal_type || form.value.dish_ids.length === 0) {
    ElMessage.warning("请填写完整信息")
    return
  }

  // 验证每餐不少于六荤六素
  let meatCount = 0
  let vegetableCount = 0
  
  for (const dish of dishList.value) {
    if (form.value.dish_ids.includes(Number(dish.dish_id))) {
      if (dish.type === "荤") {
        meatCount++
      } else if (dish.type === "素") {
        vegetableCount++
      }
    }
  }
  
  if (meatCount < 6 || vegetableCount < 6) {
    ElMessage.error(`餐单设置不符合标准：需要至少6荤6素，当前选择了${meatCount}荤${vegetableCount}素`)
    return
  }

  const res = await axios.post(
    'http://192.168.56.100:8080/menuCreate',
    form.value
  )

  if (res.data.code === 0) {
    ElMessage.success("创建成功")
    dialogVisible.value = false
    getMenus()
  } else {
    ElMessage.error(res.data.msg || "创建失败")
  }
}

// ================= 删除 =================
const deleteMenu = async (menu_id) => {
  await axios.post('http://192.168.56.100:8080/menuDelete', { menu_id })
  ElMessage.success("删除成功")
  getMenus()
}

// 初始化
onMounted(() => {
  getMenus()
  getDishes()
})
</script>