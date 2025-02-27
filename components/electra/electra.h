#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
    namespace electra {

        // Temperature
        const float RC3_TEMP_MIN = 16.0;
        const float RC3_TEMP_MAX = 30.0;
        typedef union ElectraCode {
            // That configuration has a total of 34 bits
            //    33: Power bit, if this bit is ON, the A/C will toggle it's power.
            // 32-30: Mode - Cool, heat etc.
            // 29-28: Fan - Low, medium etc.
            //    27: iFeel, bit 24 tells the ac if iFeel is on or not, and this bit, tells if the current command need to be read as an iFeel(temp 19 - 23 ) or change(temp 19 - 22)
            //    26: Zero
            //    25: Swing On/Off
            //    24: iFeel On/Off
            //    23: iFeel tempbit, iFeel uses additonal bit for temp, and 5 is added.
            // 22-19: Universl Temp, used for both controling the temp and iFeel, for controling the value is increesed by 15
            //    18: Sleep mode On/Off
            // 17- 2: Zeros
            //     1: One
            //     0: Zero
            uint64_t num;
            struct {
                uint64_t zeros1 : 1;
                uint64_t ones1 : 1;
                uint64_t zeros2 : 16;
                uint64_t sleep : 1;
                uint64_t temperature : 4;
                uint64_t ifeel_temp : 1;
                uint64_t ifeel : 1;
                uint64_t swing : 1;
                uint64_t zeros3 : 1;
                uint64_t ifeel_oriented : 1;
                uint64_t fan : 2;
                uint64_t mode : 3;
                uint64_t power : 1;
            };
        } ElectraCode;
        typedef enum IRElectraFan {
            IRElectraFanLow = 0b00,
            IRElectraFanMedium = 0b01,
            IRElectraFanHigh = 0b10,
            IRElectraFanAuto = 0b11
        } IRElectraFan;
        typedef enum IRElectraMode {
            IRElectraModeCool = 0b001,
            IRElectraModeHeat = 0b010,
            IRElectraModeAuto = 0b011,
            IRElectraModeDry = 0b100,
            IRElectraModeFan = 0b101,
            IRElectraModeOff = 0b111
        } IRElectraMode;


        class ElectraClimate : public climate_ir::ClimateIR {
            public:
            ElectraClimate()
                : climate_ir::ClimateIR(RC3_TEMP_MIN, RC3_TEMP_MAX, 1.0f, false, false,
                { climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
                climate::CLIMATE_FAN_HIGH },
                { climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL }) {}

            binary_sensor::BinarySensor *hvacState_{nullptr};
            void set_hvacState(binary_sensor::BinarySensor *binarySensorHvacState) {
                this->hvacState_ = binarySensorHvacState;
            }
            void setup() override;
            void setOffSupport(bool supports);
            void sync_state();

            protected:

            /// declartion of variables
            static const int32_t ELECTRA_TIME_UNIT = 1000;
            static const int32_t ELECTRA_NUM_BITS = 34;
            bool supportsOff;
            int32_t last_transmit_time_{};
            /// end declartion


          /// Transmit via IR the state of this climate controller.
            void transmit_state() override;
            /// Handle received IR Buffer
            bool on_receive(remote_base::RemoteReceiveData data) override;
            /// override control
            void control(const climate::ClimateCall& call) override;
            /// decodes the incoming signal
            ElectraCode decode_electra(remote_base::RemoteReceiveData data);
            ElectraCode analyze_electra(remote_base::RemoteReceiveData& data);
        };




    }  // namespace electra
}  //namespace esphome
