<template>
  <div class="cardholder">
    <SNTPCard></SNTPCard>
    <CronCard></CronCard>
    <MQTT1Card></MQTT1Card>
  </div>
</template>

<script setup>
import SNTPCard from "components/webguicomp/cards/SNTPCard.vue";
import MQTT1Card from "components/webguicomp/cards/MQTT1Card.vue";
import CronCard from "src/components/webguicomp/cards/CronCard.vue";


defineOptions({
  name: 'ServicesPage'
})
</script>
