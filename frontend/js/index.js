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

        $(".close_win").click();
        alert("欢迎回来，" + userAccount);
        
        getTaskList();
    });



    //发布任务
    $(".Createtk").on("click", function() {
        console.log("发布任务");
        console.log(userPrivateKey);
        $("#ReleaseTask").modal(); 
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



    //提交任务
    $(".Commit").on("click", function() {
        console.log("提交任务");
        $("#SubmitTask").modal();
    });

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
    });

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
                           })
          .catch(error => {console.error(error);alert("发生错误！" + error)});

    }); 
    


    //查看余额
    $(".GetBalance").on("click", function() {
        eos.getCurrencyBalance({
            account: userAccount,
            code: 'pdjtoken',
            symbol: 'PTK'
        })
        .then(result => {   console.log(result);
                            if(result.length == 0)
                                alert(userAccount + ":余额为0");
                            else
                                alert(userAccount +": " + result);
                        })
        .catch(error => console.error(error));
        getTaskList();
    })

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
            for(var i = 0; i < result.rows.length; i++){
                
                tr += '<tr>';
                tr += '<td class="active">'+result.rows[i].taskID+'</td>';
                tr += '<td class="success">'+result.rows[i].creator+'</td>';
                tr += '<td class="warning">'+result.rows[i].worker+'</td>';
                tr += '<td class="danger">'+result.rows[i].bonus+'</td>';
                tr += '<td class="info">'+result.rows[i].status+'</td>';
                tr += '<td class="active">'+result.rows[i].remark+'</td>';
                tr += '<td class="success">'+result.rows[i].comment+'</td>';
                tr += '</tr>';
                
            }
            console.log(tr);
            $("#list").html(tr);
            
        })
        .catch(error => console.error(error));    
    }
    
});