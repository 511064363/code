/*
 Navicat MySQL Data Transfer

 Source Server         : localhost
 Source Server Type    : MySQL
 Source Server Version : 80024
 Source Host           : localhost:3306
 Source Schema         : db_mms

 Target Server Type    : MySQL
 Target Server Version : 80024
 File Encoding         : 65001

 Date: 30/04/2021 14:59:38
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for lrs_role
-- ----------------------------
DROP TABLE IF EXISTS `lrs_role`;
CREATE TABLE `lrs_role`  (
  `rid` int NOT NULL AUTO_INCREMENT,
  `name` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '角色昵称',
  `power` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '权限（1:用户管理2:角色管理3:视频管理）',
  PRIMARY KEY (`rid`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 4 CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '用户角色表' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of lrs_role
-- ----------------------------
INSERT INTO `lrs_role` VALUES (1, '医生', '');
INSERT INTO `lrs_role` VALUES (2, '管理员', '1');
INSERT INTO `lrs_role` VALUES (3, '超级管理员', '1,2,3');

-- ----------------------------
-- Table structure for lrs_user
-- ----------------------------
DROP TABLE IF EXISTS `lrs_user`;
CREATE TABLE `lrs_user`  (
  `uid` bigint NOT NULL AUTO_INCREMENT COMMENT '用户ID',
  `username` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '用户登录名',
  `password` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '用户登陆密码',
  `name` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '用户姓名',
  `sex` int NOT NULL DEFAULT 1 COMMENT '用户性别 1:男 2:女',
  `phone` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '用户手机',
  `role_id` int NOT NULL DEFAULT 0 COMMENT '角色ID',
  `open_uid` bigint NOT NULL DEFAULT 0 COMMENT '开通用户UID',
  `add_time` bigint NOT NULL DEFAULT 0 COMMENT '新增时间',
  `add_ip` varchar(50) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '' COMMENT '操作IP',
  `isdisabled` int NOT NULL DEFAULT 0 COMMENT '禁用标识 0:禁用 1:正常',
  PRIMARY KEY (`uid`) USING BTREE,
  UNIQUE INDEX `user_username`(`username`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 15 CHARACTER SET = utf8 COLLATE = utf8_general_ci COMMENT = '系统用户列表' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of lrs_user
-- ----------------------------
INSERT INTO `lrs_user` VALUES (1, 'bigadmin', '55f5208742036532e6813fd0305e3a97', '超级大管理', 2, '176230863761', 3, 1, 0, '', 1);
INSERT INTO `lrs_user` VALUES (11, 'admin', '2d1445c452a3229714feac17903aa4e9', 'admin', 1, '13888888888', 1, 1, 1530845629196, '192.168.30.123', 1);
INSERT INTO `lrs_user` VALUES (14, 'mdsd', 'b1fdf608330064042e080b63e12ba916', 'mdsd', 1, '', 3, 1, 0, '', 1);

-- ----------------------------
-- Table structure for tb_custom_input
-- ----------------------------
DROP TABLE IF EXISTS `tb_custom_input`;
CREATE TABLE `tb_custom_input`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '用户分类主键',
  `content` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '用户自定义输入的内容',
  `is_disable` int NOT NULL DEFAULT 0 COMMENT '是否被禁用',
  `add_time` datetime NULL DEFAULT NULL,
  `is_delete` int NOT NULL DEFAULT 0 COMMENT '该数据是否被删除',
  `classify` int UNSIGNED NOT NULL DEFAULT 0,
  `adder` bigint NOT NULL COMMENT '谁添加的',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `fk_classify`(`classify`) USING BTREE,
  INDEX `fk_adder`(`adder`) USING BTREE,
  CONSTRAINT `fk_adder` FOREIGN KEY (`adder`) REFERENCES `lrs_user` (`uid`) ON DELETE RESTRICT ON UPDATE RESTRICT,
  CONSTRAINT `fk_classify` FOREIGN KEY (`classify`) REFERENCES `tb_custom_input_classification` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE = InnoDB AUTO_INCREMENT = 97 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of tb_custom_input
-- ----------------------------

-- ----------------------------
-- Table structure for tb_custom_input_classification
-- ----------------------------
DROP TABLE IF EXISTS `tb_custom_input_classification`;
CREATE TABLE `tb_custom_input_classification`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT COMMENT '用户自定义的输入下拉提示列表分类',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL COMMENT '分类名',
  `add_time` datetime NULL DEFAULT NULL COMMENT '添加时间',
  `is_disable` int NOT NULL DEFAULT 0 COMMENT '该分类是否被禁用，如果该分类被禁用，分类下的所有值都会被禁用',
  `is_delete` int NOT NULL DEFAULT 0 COMMENT '是否被删除',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 7 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of tb_custom_input_classification
-- ----------------------------
INSERT INTO `tb_custom_input_classification` VALUES (1, '送检医生', '2018-06-14 11:17:48', 0, 0);
INSERT INTO `tb_custom_input_classification` VALUES (2, '送检科室', '2018-06-14 11:31:02', 0, 0);
INSERT INTO `tb_custom_input_classification` VALUES (3, '手术医生', '2018-06-14 11:31:28', 0, 0);
INSERT INTO `tb_custom_input_classification` VALUES (4, '器械护士', '2018-06-14 11:33:35', 0, 0);
INSERT INTO `tb_custom_input_classification` VALUES (5, '手术助手', '2018-06-14 11:36:50', 0, 0);
INSERT INTO `tb_custom_input_classification` VALUES (6, '麻醉方式', '2018-06-14 11:37:11', 0, 0);

-- ----------------------------
-- Table structure for tb_patient
-- ----------------------------
DROP TABLE IF EXISTS `tb_patient`;
CREATE TABLE `tb_patient`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `parent_id` int(11) UNSIGNED ZEROFILL NOT NULL DEFAULT 00000000000 COMMENT '上级病人id，如果上级病人id不为0则表示本次记录为病人复查记录',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '病人的名字',
  `sex` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '病人的性别',
  `bed_no` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '床位号',
  `age` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '0' COMMENT '病人的年龄',
  `contact_phone` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '病人的联系电话',
  `hospital_no` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '住院号',
  `outpatient_no` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '门诊号',
  `device_model_no` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '设备型号',
  `surgical_no` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '手术号',
  `contact_address` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '病人联系地址',
  `doctor` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '送诊医生',
  `department` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '送检科室',
  `surgery_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '手术名称',
  `clinical_diagnosis` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '临床诊断',
  `surgery_dcotor` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '手术医生',
  `equipment_nurse` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '器械护士',
  `surgery_assistant` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '手术助手',
  `anesthesia` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT '' COMMENT '麻醉方式',
  `surgery_description` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '手术描述',
  `surgical_record` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '手术记录',
  `pathological_diagnosis` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '病理诊断详情',
  `is_deleted` int(1) UNSIGNED ZEROFILL NOT NULL DEFAULT 0 COMMENT '是否被删除，当用户被删除时会设置为1',
  `remark` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '备注，用于记录一些附加信息，比如复诊说明',
  `add_time` datetime NULL DEFAULT NULL COMMENT '记录新增时间',
  `video_list` json NULL COMMENT '视频地址列表，存放视频的相对地址',
  `image_list` json NULL COMMENT '图片列表',
  `server_patient_id` bigint NOT NULL COMMENT '该患者信息在服务器上保存的id，如果未保存则为0',
  `jzh` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '记账号',
  `eye_gender` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `affected_part` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  `surgery_method` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL COMMENT '手术方式',
  `disease` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL DEFAULT NULL,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 465 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci COMMENT = '病患信息表' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of tb_patient
-- ----------------------------

-- ----------------------------
-- Table structure for tb_template
-- ----------------------------
DROP TABLE IF EXISTS `tb_template`;
CREATE TABLE `tb_template`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `title` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '模板的标题',
  `content` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '模板内容',
  `sort` int(11) UNSIGNED ZEROFILL NOT NULL DEFAULT 00000000000 COMMENT '用来描述排序的值，具体排序方式取决于应用程序',
  `add_time` datetime NULL DEFAULT NULL COMMENT '模板的添加时间',
  `is_delete` int NOT NULL DEFAULT 0 COMMENT '该记录是否被删除',
  `is_disable` int NOT NULL DEFAULT 0 COMMENT '该模板是否禁用',
  `modify_time` datetime NULL DEFAULT NULL COMMENT '模板的最近一次修改时间',
  `classification_id` int UNSIGNED NOT NULL DEFAULT 0 COMMENT '分类id',
  PRIMARY KEY (`id`) USING BTREE,
  INDEX `fk_classification`(`classification_id`) USING BTREE,
  CONSTRAINT `fk_classification` FOREIGN KEY (`classification_id`) REFERENCES `tb_template_classification` (`id`) ON DELETE RESTRICT ON UPDATE RESTRICT
) ENGINE = InnoDB AUTO_INCREMENT = 87 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci COMMENT = '诊断模板类，用于记录模板内容' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of tb_template
-- ----------------------------
INSERT INTO `tb_template` VALUES (1, NULL, '岁', 00000000000, '2019-06-16 14:36:05', 0, 0, '2001-01-01 00:00:00', 7);
INSERT INTO `tb_template` VALUES (2, NULL, '月', 00000000000, '2019-06-16 14:36:08', 0, 0, '2001-01-01 00:00:00', 7);
INSERT INTO `tb_template` VALUES (3, NULL, '天', 00000000000, '2019-06-16 14:36:11', 0, 0, '2001-01-01 00:00:00', 7);

-- ----------------------------
-- Table structure for tb_template_classification
-- ----------------------------
DROP TABLE IF EXISTS `tb_template_classification`;
CREATE TABLE `tb_template_classification`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `name` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NULL COMMENT '分类名',
  `add_time` datetime NULL DEFAULT NULL COMMENT '分类的添加时间',
  `is_delete` int(1) UNSIGNED ZEROFILL NOT NULL DEFAULT 0 COMMENT '是否已删除',
  `is_disable` int(1) UNSIGNED ZEROFILL NOT NULL DEFAULT 0 COMMENT '是否禁用了该模板分类',
  `sort` int NOT NULL DEFAULT 0,
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 13 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci COMMENT = '诊断模板分类表，记录模板的分类' ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of tb_template_classification
-- ----------------------------
INSERT INTO `tb_template_classification` VALUES (6, '主刀医师', '2019-06-13 18:17:14', 0, 0, 0);
INSERT INTO `tb_template_classification` VALUES (7, '年龄', '2019-06-16 01:20:49', 0, 0, -1);
INSERT INTO `tb_template_classification` VALUES (8, '手术名称', '2019-06-17 20:50:51', 0, 0, -1);
INSERT INTO `tb_template_classification` VALUES (9, '病区名称', '2019-06-17 20:51:13', 0, 0, -1);
INSERT INTO `tb_template_classification` VALUES (10, '手术方式', '2019-06-17 20:51:39', 0, 0, -1);
INSERT INTO `tb_template_classification` VALUES (11, '手术描述', '2021-04-30 14:53:15', 0, 0, 0);
INSERT INTO `tb_template_classification` VALUES (12, '图片标注', '2021-04-30 14:53:15', 0, 0, 0);

-- ----------------------------
-- Table structure for tb_word
-- ----------------------------
DROP TABLE IF EXISTS `tb_word`;
CREATE TABLE `tb_word`  (
  `id` int NOT NULL AUTO_INCREMENT COMMENT '主键',
  `group_id` int(11) UNSIGNED ZEROFILL NOT NULL DEFAULT 00000000000 COMMENT '分组id',
  `text` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci NOT NULL DEFAULT '' COMMENT '单词的内容',
  `add_time` datetime NULL DEFAULT NULL,
  `is_disable` int(1) UNSIGNED ZEROFILL NOT NULL DEFAULT 0 COMMENT '是否禁用',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB CHARACTER SET = utf8mb4 COLLATE = utf8mb4_unicode_ci ROW_FORMAT = DYNAMIC;

-- ----------------------------
-- Records of tb_word
-- ----------------------------

SET FOREIGN_KEY_CHECKS = 1;
