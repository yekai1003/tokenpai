$(function() {
    var userAccount;
    var userPrivateKey;
    $("#LoginWindow").modal();

    $(".Login").on("click", function() {
        userAccount = $("#userAcc").val();
        userPrivateKey = $("#PrivateKey").val();
        
        config = {
            chainId: 'cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4f', // 32 byte (64 char) hex string
            keyProvider: [userPrivateKey], // WIF string or array of keys..   //5J2ZC6ZbtoFnTsZofTnMaTQmZSkdx9DnM9QZbuYTvDS2AgQaGzX
            httpEndpoint: 'http://127.0.0.1:8888',
            expireInSeconds: 60,
            broadcast: true,
            verbose: false, // API activity
            sign: true
        }        
        eos = Eos(config);


        eos.getAccount(userAccount).then(result => {
            console.log(result);
            alert("欢迎回来，" + userAccount);
            $(".userName span:nth-child(2)").html("账户：" + userAccount);
            getBalance();
        }).catch(err => {
            console.log(err);
            alert("错误：账户不存在！");
        });



        $(".close_win").click();
        getTaskList();
    });





    //发布任务
    $(".Createtk").on("click", function() {
        console.log("发布任务");
        console.log(userPrivateKey);
        $("#ReleaseTask").modal(); 
        getTaskList();
    });
    //确认发布任务
    $(".ConfirmRelease").on("click", function() {
        var WorkerAcc  = $("#GetWorker").val();
        var TaskToken  = $("#GetToken").val();
        var TaskInfo   = $("#GetTaskInfo").val();
        console.log(WorkerAcc,TaskToken,TaskInfo);
        $(".close_win").click();
    
        eos.transaction({
            actions: [
            {
                account: 'pdjtask',
                name:    'createtk',
                authorization: [{
                    actor:      userAccount,
                    permission: 'active'
                }],
                    
                data: {
                    creator:    userAccount,
                    worker:     WorkerAcc,
                    taskBonus:  TaskToken,
                    memo:       TaskInfo
                }
            }
            ]
        }).then(result => {
                                console.log(result);
                                alert("发布成功！");
                                getTaskList();  
                          })
          .catch(error => {console.error(error);alert("发生错误！" + error)});
    });
    
    





    //领取任务
    $(".Receive").on("click", function() {
        console.log("领取任务");
        $("#ReceiveTask").modal();
        getTaskList();
    });
    //确认领取
    $(".ConfirmReceive").on("click", function() {
        var TaskID = $("#ReceiveTaskID").val();
        console.log(TaskID);
        $(".close_win").click();

        eos.transaction({
            actions: [
            {
                account: 'pdjtask',
                name:    'receivetk',
                authorization: [{
                    actor:      userAccount,
                    permission: 'active'
                }],
                    
                data: {
                    taskID:     TaskID,
                    worker:     userAccount
                }
            }
            ]
        }).then(result => {
                                console.log(result);
                                alert("领取成功");
                                getTaskList();  
                          })
          .catch(error => {console.error(error);alert("发生错误！" + error)});
    });





    //提交任务
    $(".Commit").on("click", function() {
        console.log("提交任务");
        $("#SubmitTask").modal();
        getTaskList();
    });
    //确认提交
    $(".ConfirmSubmission").on("click", function() {

        var TaskID  = $("#GetTaskID").val();
        var TaskInfo = $("#TaskInformation").val();
        console.log(TaskInfo);
        $(".close_win").click();
        eos.transaction({
            actions: [
                {
                    account: 'pdjtask',
                    name:    'commit',
                    authorization: [{
                        actor:      userAccount,
                        permission: 'active'
                    }],
                    
                    data: {
                        taskID:     TaskID,    
                        memo:       TaskInfo      
                    }
                }
            ]
        }).then(result => {     console.log(result);
                                alert("提交成功");
                                getTaskList();
                          })
          .catch(error => {console.error(error);alert("发生错误！" + error)});
    }); 


    //验收任务
    $(".Confirm").on("click", function() {
        console.log("验收任务");
        $("#ConfirmTask").modal();
        getTaskList();
    });
    //确认验收
    $(".TaskConfirm").on("click", function() {

        var TaskID  = $("#taskid").val();
        var TaskStatus = $("#TaskStatus").val();
        TaskStatus = parseInt(TaskStatus);
        console.log(TaskID,TaskStatus);
        $(".close_win").click();
        eos.transaction({
            actions: [
                {
                    account: 'pdjtask',
                    name:    'confirm',
                    authorization: [{
                        actor:      userAccount,
                        permission: 'active'
                    }],
                    
                    data: {
                        taskID:     TaskID,    
                        ok:         TaskStatus
                    }
                }
            ]
        }).then(result => { 
                             console.log(result);
                             alert("任务验收成功");
                             getTaskList();
                             getBalance();
                           })
          .catch(error => {console.error(error);alert("发生错误！" + error)});

    }); 
    


 
    //查看余额
    function getBalance(){
        eos.getCurrencyBalance({
            account: userAccount,
            code: 'pdjtoken',
            symbol: 'PTK'
        })
        .then(result => {   console.log(result);
                            if(result.length == 0)
                                $(".balance span:nth-child(2)").html("余额：0");
                            else
                                $(".balance span:nth-child(2)").html("余额：" + result);
                        })
        .catch(error => console.error(error));
    }

    //console.log(eos);
  
  
    //任务列表
    function getTaskList(){
        eos.getTableRows({
            scope:  'pdjtask',
            code:   'pdjtask',
            table:  'tasks',
            json:   true,
            lower_bound: 0,
            upper_bound: -1,
            limit:  20
        })    
        .then(function(result){
            console.log(result.rows);
            var tr;
            var tkStatus = "";
            for(var i = 0; i < result.rows.length; i++){
                
                if(result.rows[i].status == 0)
                    tkStatus = "未领取";
                else if(result.rows[i].status == 1)
                    tkStatus = "已领取";
                else if(result.rows[i].status == 2)
                    tkStatus = "已提交";
                else
                    tkStatus = "已结束";

                tr += '<tr>';
                tr += '<td class="active">'+result.rows[i].taskID+'</td>';
                tr += '<td class="success">'+result.rows[i].creator+'</td>';
                tr += '<td class="warning">'+result.rows[i].worker+'</td>';
                tr += '<td class="danger">'+result.rows[i].bonus+'</td>';
                tr += '<td class="info">'+tkStatus+'</td>';
                tr += '<td class="active">'+result.rows[i].remark+'</td>';
                tr += '<td class="success">'+result.rows[i].comment+'</td>';
                tr += '</tr>';
                
            }
            //console.log(tr);
            $("#list").html(tr);
            
        })
        .catch(error => console.error(error));    
    }
    
});