/**
 *
 *  Settings.h
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#pragma once
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <drogon/orm/Field.h>
#include <drogon/orm/SqlBinder.h>
#include <drogon/orm/Mapper.h>
#ifdef __cpp_impl_coroutine
#include <drogon/orm/CoroMapper.h>
#endif
#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <json/json.h>
#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include <stdint.h>
#include <iostream>

namespace drogon
{
namespace orm
{
class DbClient;
using DbClientPtr = std::shared_ptr<DbClient>;
}
}
namespace drogon_model
{
namespace sqlite3
{

class Settings
{
  public:
    struct Cols
    {
        static const std::string _id;
        static const std::string _distance_steel;
        static const std::string _distance_camera;
        static const std::string _camera_address;
        static const std::string _update_time;
        static const std::string _device_name;
        static const std::string _custom_param;
        static const std::string _algorithm_version;
        static const std::string _systemVersion;
        static const std::string _device_version;
    };

    const static int primaryKeyNumber;
    const static std::string tableName;
    const static bool hasPrimaryKey;
    const static std::string primaryKeyName;
    using PrimaryKeyType = uint64_t;
    const PrimaryKeyType &getPrimaryKey() const;

    /**
     * @brief constructor
     * @param r One row of records in the SQL query result.
     * @param indexOffset Set the offset to -1 to access all columns by column names,
     * otherwise access all columns by offsets.
     * @note If the SQL is not a style of 'select * from table_name ...' (select all
     * columns by an asterisk), please set the offset to -1.
     */
    explicit Settings(const drogon::orm::Row &r, const ssize_t indexOffset = 0) noexcept;

    /**
     * @brief constructor
     * @param pJson The json object to construct a new instance.
     */
    explicit Settings(const Json::Value &pJson) noexcept(false);

    /**
     * @brief constructor
     * @param pJson The json object to construct a new instance.
     * @param pMasqueradingVector The aliases of table columns.
     */
    Settings(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false);

    Settings() = default;

    void updateByJson(const Json::Value &pJson) noexcept(false);
    void updateByMasqueradedJson(const Json::Value &pJson,
                                 const std::vector<std::string> &pMasqueradingVector) noexcept(false);
    static bool validateJsonForCreation(const Json::Value &pJson, std::string &err);
    static bool validateMasqueradedJsonForCreation(const Json::Value &,
                                                const std::vector<std::string> &pMasqueradingVector,
                                                    std::string &err);
    static bool validateJsonForUpdate(const Json::Value &pJson, std::string &err);
    static bool validateMasqueradedJsonForUpdate(const Json::Value &,
                                          const std::vector<std::string> &pMasqueradingVector,
                                          std::string &err);
    static bool validJsonOfField(size_t index,
                          const std::string &fieldName,
                          const Json::Value &pJson,
                          std::string &err,
                          bool isForCreation);

    /**  For column id  */
    ///Get the value of the column id, returns the default value if the column is null
    const uint64_t &getValueOfId() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<uint64_t> &getId() const noexcept;
    ///Set the value of the column id
    void setId(const uint64_t &pId) noexcept;

    /**  For column distance_steel  */
    ///Get the value of the column distance_steel, returns the default value if the column is null
    const std::string &getValueOfDistanceSteel() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getDistanceSteel() const noexcept;
    ///Set the value of the column distance_steel
    void setDistanceSteel(const std::string &pDistanceSteel) noexcept;
    void setDistanceSteel(std::string &&pDistanceSteel) noexcept;
    void setDistanceSteelToNull() noexcept;

    /**  For column distance_camera  */
    ///Get the value of the column distance_camera, returns the default value if the column is null
    const std::string &getValueOfDistanceCamera() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getDistanceCamera() const noexcept;
    ///Set the value of the column distance_camera
    void setDistanceCamera(const std::string &pDistanceCamera) noexcept;
    void setDistanceCamera(std::string &&pDistanceCamera) noexcept;
    void setDistanceCameraToNull() noexcept;

    /**  For column camera_address  */
    ///Get the value of the column camera_address, returns the default value if the column is null
    const std::string &getValueOfCameraAddress() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getCameraAddress() const noexcept;
    ///Set the value of the column camera_address
    void setCameraAddress(const std::string &pCameraAddress) noexcept;
    void setCameraAddress(std::string &&pCameraAddress) noexcept;
    void setCameraAddressToNull() noexcept;

    /**  For column update_time  */
    ///Get the value of the column update_time, returns the default value if the column is null
    const std::string &getValueOfUpdateTime() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getUpdateTime() const noexcept;
    ///Set the value of the column update_time
    void setUpdateTime(const std::string &pUpdateTime) noexcept;
    void setUpdateTime(std::string &&pUpdateTime) noexcept;
    void setUpdateTimeToNull() noexcept;

    /**  For column device_name  */
    ///Get the value of the column device_name, returns the default value if the column is null
    const std::string &getValueOfDeviceName() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getDeviceName() const noexcept;
    ///Set the value of the column device_name
    void setDeviceName(const std::string &pDeviceName) noexcept;
    void setDeviceName(std::string &&pDeviceName) noexcept;
    void setDeviceNameToNull() noexcept;

    /**  For column custom_param  */
    ///Get the value of the column custom_param, returns the default value if the column is null
    const std::string &getValueOfCustomParam() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getCustomParam() const noexcept;
    ///Set the value of the column custom_param
    void setCustomParam(const std::string &pCustomParam) noexcept;
    void setCustomParam(std::string &&pCustomParam) noexcept;
    void setCustomParamToNull() noexcept;

    /**  For column algorithm_version  */
    ///Get the value of the column algorithm_version, returns the default value if the column is null
    const std::string &getValueOfAlgorithmVersion() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getAlgorithmVersion() const noexcept;
    ///Set the value of the column algorithm_version
    void setAlgorithmVersion(const std::string &pAlgorithmVersion) noexcept;
    void setAlgorithmVersion(std::string &&pAlgorithmVersion) noexcept;
    void setAlgorithmVersionToNull() noexcept;

    /**  For column systemVersion  */
    ///Get the value of the column systemVersion, returns the default value if the column is null
    const std::string &getValueOfSystemversion() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getSystemversion() const noexcept;
    ///Set the value of the column systemVersion
    void setSystemversion(const std::string &pSystemversion) noexcept;
    void setSystemversion(std::string &&pSystemversion) noexcept;
    void setSystemversionToNull() noexcept;

    /**  For column device_version  */
    ///Get the value of the column device_version, returns the default value if the column is null
    const std::string &getValueOfDeviceVersion() const noexcept;
    ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
    const std::shared_ptr<std::string> &getDeviceVersion() const noexcept;
    ///Set the value of the column device_version
    void setDeviceVersion(const std::string &pDeviceVersion) noexcept;
    void setDeviceVersion(std::string &&pDeviceVersion) noexcept;
    void setDeviceVersionToNull() noexcept;


    static size_t getColumnNumber() noexcept {  return 10;  }
    static const std::string &getColumnName(size_t index) noexcept(false);

    Json::Value toJson() const;
    Json::Value toMasqueradedJson(const std::vector<std::string> &pMasqueradingVector) const;
    /// Relationship interfaces
  private:
    friend drogon::orm::Mapper<Settings>;
#ifdef __cpp_impl_coroutine
    friend drogon::orm::CoroMapper<Settings>;
#endif
    static const std::vector<std::string> &insertColumns() noexcept;
    void outputArgs(drogon::orm::internal::SqlBinder &binder) const;
    const std::vector<std::string> updateColumns() const;
    void updateArgs(drogon::orm::internal::SqlBinder &binder) const;
    ///For mysql or sqlite3
    void updateId(const uint64_t id);
    std::shared_ptr<uint64_t> id_;
    std::shared_ptr<std::string> distanceSteel_;
    std::shared_ptr<std::string> distanceCamera_;
    std::shared_ptr<std::string> cameraAddress_;
    std::shared_ptr<std::string> updateTime_;
    std::shared_ptr<std::string> deviceName_;
    std::shared_ptr<std::string> customParam_;
    std::shared_ptr<std::string> algorithmVersion_;
    std::shared_ptr<std::string> systemversion_;
    std::shared_ptr<std::string> deviceVersion_;
    struct MetaData
    {
        const std::string colName_;
        const std::string colType_;
        const std::string colDatabaseType_;
        const ssize_t colLength_;
        const bool isAutoVal_;
        const bool isPrimaryKey_;
        const bool notNull_;
    };
    static const std::vector<MetaData> metaData_;
    bool dirtyFlag_[10]={ false };
  public:
    static const std::string &sqlForFindingByPrimaryKey()
    {
        static const std::string sql="select * from " + tableName + " where id = ?";
        return sql;
    }

    static const std::string &sqlForDeletingByPrimaryKey()
    {
        static const std::string sql="delete from " + tableName + " where id = ?";
        return sql;
    }
    std::string sqlForInserting(bool &needSelection) const
    {
        std::string sql="insert into " + tableName + " (";
        size_t parametersCount = 0;
        needSelection = false;
        if(dirtyFlag_[1])
        {
            sql += "distance_steel,";
            ++parametersCount;
        }
        if(dirtyFlag_[2])
        {
            sql += "distance_camera,";
            ++parametersCount;
        }
        if(dirtyFlag_[3])
        {
            sql += "camera_address,";
            ++parametersCount;
        }
        if(dirtyFlag_[4])
        {
            sql += "update_time,";
            ++parametersCount;
        }
        if(dirtyFlag_[5])
        {
            sql += "device_name,";
            ++parametersCount;
        }
        if(dirtyFlag_[6])
        {
            sql += "custom_param,";
            ++parametersCount;
        }
        if(dirtyFlag_[7])
        {
            sql += "algorithm_version,";
            ++parametersCount;
        }
        if(dirtyFlag_[8])
        {
            sql += "systemVersion,";
            ++parametersCount;
        }
        if(dirtyFlag_[9])
        {
            sql += "device_version,";
            ++parametersCount;
        }
        if(parametersCount > 0)
        {
            sql[sql.length()-1]=')';
            sql += " values (";
        }
        else
            sql += ") values (";

        if(dirtyFlag_[1])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[2])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[3])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[4])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[5])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[6])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[7])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[8])
        {
            sql.append("?,");

        }
        if(dirtyFlag_[9])
        {
            sql.append("?,");

        }
        if(parametersCount > 0)
        {
            sql.resize(sql.length() - 1);
        }
        sql.append(1, ')');
        LOG_TRACE << sql;
        return sql;
    }
};
} // namespace sqlite3
} // namespace drogon_model
