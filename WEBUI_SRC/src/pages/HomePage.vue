<template>
  <div class="cardholder">
    <ColorCard></ColorCard>
    <FadeCard></FadeCard>
  </div>
</template>

<script setup>
import ColorCard from "src/components/ColorCard.vue";
import FadeCard from "src/components/webguicomp/cards/FadeCard.vue";

defineOptions({
  name: 'HomePage'
})
</script>
